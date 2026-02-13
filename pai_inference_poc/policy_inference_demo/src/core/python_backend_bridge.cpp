/**
 * @file python_backend_bridge.cpp
 * @brief Python bridge backend implementation for dynamic Python inference classes.
 */

#include "policy_inference_demo/core/python_backend_bridge.hpp"

#include <Python.h>

#include <mutex>
#include <string>

#include "pluginlib/class_list_macros.hpp"
#include "rclcpp/logging.hpp"

namespace policy_inference_demo
{

namespace
{
rclcpp::Logger kLogger = rclcpp::get_logger("policy_inference_demo.python_backend_bridge");
std::mutex kPythonInitMutex;

bool parse_output_sequence(PyObject * obj, std::vector<double> & outputs)
{
  if (!PyList_Check(obj) && !PyTuple_Check(obj))
  {
    return false;
  }

  const Py_ssize_t size = PySequence_Size(obj);
  if (size < 0)
  {
    return false;
  }
  outputs.clear();
  outputs.reserve(static_cast<std::size_t>(size));

  for (Py_ssize_t i = 0; i < size; ++i)
  {
    PyObject * item = PySequence_GetItem(obj, i);  // New reference.
    if (!item)
    {
      return false;
    }
    const double value = PyFloat_AsDouble(item);
    Py_DECREF(item);
    if (PyErr_Occurred())
    {
      PyErr_Clear();
      return false;
    }
    outputs.push_back(value);
  }
  return true;
}
}  // namespace

struct PythonBackendBridge::Impl
{
  PyObject * module = nullptr;
  PyObject * instance = nullptr;
  std::mutex mutex;
};

PythonBackendBridge::PythonBackendBridge()
: impl_(std::make_unique<Impl>())
{
}

PythonBackendBridge::~PythonBackendBridge()
{
  if (!impl_)
  {
    return;
  }

  std::scoped_lock<std::mutex> lock(impl_->mutex);
  if (Py_IsInitialized() == 0)
  {
    impl_->module = nullptr;
    impl_->instance = nullptr;
    return;
  }

  PyGILState_STATE gil = PyGILState_Ensure();
  Py_XDECREF(impl_->instance);
  Py_XDECREF(impl_->module);
  impl_->instance = nullptr;
  impl_->module = nullptr;
  PyGILState_Release(gil);
}

bool PythonBackendBridge::initialize(const InferenceBackendConfig & config)
{
  config_ = config;
  initialized_ = false;
  backend_id_ = "python_backend_bridge";

  if (config_.python_module.empty() || config_.python_class.empty())
  {
    RCLCPP_ERROR(
      kLogger,
      "Python backend requires 'python_module' and 'python_class' parameters.");
    return false;
  }

  std::scoped_lock<std::mutex> lock(impl_->mutex);

  {
    std::scoped_lock<std::mutex> init_lock(kPythonInitMutex);
    if (Py_IsInitialized() == 0)
    {
      Py_Initialize();
      // Release GIL so other threads can use PyGILState_Ensure without deadlocking.
      PyEval_SaveThread();
    }
  }

  PyGILState_STATE gil = PyGILState_Ensure();

  Py_XDECREF(impl_->instance);
  Py_XDECREF(impl_->module);
  impl_->instance = nullptr;
  impl_->module = nullptr;

  impl_->module = PyImport_ImportModule(config_.python_module.c_str());
  if (!impl_->module)
  {
    PyErr_Print();
    RCLCPP_ERROR(
      kLogger,
      "Failed to import Python module '%s'.",
      config_.python_module.c_str());
    PyGILState_Release(gil);
    return false;
  }

  PyObject * cls = PyObject_GetAttrString(impl_->module, config_.python_class.c_str());
  if (!cls || PyCallable_Check(cls) == 0)
  {
    Py_XDECREF(cls);
    PyErr_Print();
    RCLCPP_ERROR(
      kLogger,
      "Python class '%s' was not found or is not callable.",
      config_.python_class.c_str());
    PyGILState_Release(gil);
    return false;
  }

  impl_->instance = PyObject_CallObject(cls, nullptr);
  Py_DECREF(cls);
  if (!impl_->instance)
  {
    PyErr_Print();
    RCLCPP_ERROR(
      kLogger,
      "Failed to instantiate Python class '%s'.",
      config_.python_class.c_str());
    PyGILState_Release(gil);
    return false;
  }

  PyObject * init_result = PyObject_CallMethod(
    impl_->instance,
    "initialize",
    "sidd",
    config_.model_uri.c_str(),
    static_cast<int>(config_.output_size),
    config_.output_scale,
    config_.output_bias);
  if (!init_result)
  {
    PyErr_Print();
    RCLCPP_ERROR(
      kLogger,
      "Python backend initialize() failed for '%s.%s'.",
      config_.python_module.c_str(),
      config_.python_class.c_str());
    PyGILState_Release(gil);
    return false;
  }

  const int init_ok = PyObject_IsTrue(init_result);
  Py_DECREF(init_result);
  if (init_ok != 1)
  {
    RCLCPP_ERROR(
      kLogger,
      "Python backend initialize() returned false.");
    PyGILState_Release(gil);
    return false;
  }

  PyObject * id_result = PyObject_CallMethod(impl_->instance, "id", nullptr);
  if (id_result && PyUnicode_Check(id_result))
  {
    backend_id_ = PyUnicode_AsUTF8(id_result);
  }
  Py_XDECREF(id_result);

  initialized_ = true;
  PyGILState_Release(gil);
  return true;
}

bool PythonBackendBridge::infer(const InferenceRequest & request, InferenceResponse & response)
{
  if (!initialized_)
  {
    return false;
  }

  std::scoped_lock<std::mutex> lock(impl_->mutex);
  if (!impl_->instance)
  {
    return false;
  }

  PyGILState_STATE gil = PyGILState_Ensure();

  PyObject * py_features = PyList_New(static_cast<Py_ssize_t>(request.features.size()));
  if (!py_features)
  {
    PyGILState_Release(gil);
    return false;
  }

  for (std::size_t i = 0U; i < request.features.size(); ++i)
  {
    PyObject * value = PyFloat_FromDouble(request.features[i]);
    if (!value)
    {
      Py_DECREF(py_features);
      PyGILState_Release(gil);
      return false;
    }
    PyList_SET_ITEM(py_features, static_cast<Py_ssize_t>(i), value);  // Steals reference.
  }

  PyObject * infer_result = PyObject_CallMethod(impl_->instance, "infer", "O", py_features);
  Py_DECREF(py_features);
  if (!infer_result)
  {
    PyErr_Print();
    RCLCPP_ERROR(kLogger, "Python backend infer() failed.");
    PyGILState_Release(gil);
    return false;
  }

  response.outputs.clear();
  response.backend_id = backend_id_;

  if (PyDict_Check(infer_result))
  {
    PyObject * outputs_obj = PyDict_GetItemString(infer_result, "outputs");  // Borrowed.
    if (!outputs_obj || !parse_output_sequence(outputs_obj, response.outputs))
    {
      Py_DECREF(infer_result);
      RCLCPP_ERROR(kLogger, "Python infer() dict result must contain numeric 'outputs'.");
      PyGILState_Release(gil);
      return false;
    }

    PyObject * backend_id_obj = PyDict_GetItemString(infer_result, "backend_id");  // Borrowed.
    if (backend_id_obj && PyUnicode_Check(backend_id_obj))
    {
      response.backend_id = PyUnicode_AsUTF8(backend_id_obj);
    }
  }
  else if (!parse_output_sequence(infer_result, response.outputs))
  {
    Py_DECREF(infer_result);
    RCLCPP_ERROR(
      kLogger,
      "Python infer() return type must be dict {'outputs': [...]} or a numeric sequence.");
    PyGILState_Release(gil);
    return false;
  }

  Py_DECREF(infer_result);
  PyGILState_Release(gil);
  return true;
}

std::string PythonBackendBridge::id() const
{
  return backend_id_;
}

}  // namespace policy_inference_demo

PLUGINLIB_EXPORT_CLASS(
  policy_inference_demo::PythonBackendBridge,
  policy_inference_demo::InferenceBackendBase)
