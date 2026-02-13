/**
 * @file ros2_control_policy_inference.cpp
 * @brief Thin `ros2_control` controller that loads and calls a backend inference plugin.
 *
 * Reference implementation patterns:
 * 1. `https://github.com/ros-controls/kinematics_interface`
 * 2. `kinematics_interface/kinematics_interface/test/kinematics_interface_common_tests.hpp`
 */

#include "policy_inference_demo/interface/ros2_control_policy_inference.hpp"

#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "pluginlib/class_list_macros.hpp"
#include "pluginlib/exceptions.hpp"
#include "rclcpp/logging.hpp"

namespace policy_inference_demo
{

namespace
{
rclcpp::Logger kLogger = rclcpp::get_logger("policy_inference_demo.ros2_control_controller");
}  // namespace

controller_interface::CallbackReturn Ros2ControlPolicyInference::on_init()
{
  auto_declare<std::string>("backend_plugin", "policy_inference_demo/ExampleCppBackend");
  auto_declare<std::string>("model_uri", "example_model.onnx");
  auto_declare<int>("output_size", 3);
  auto_declare<double>("output_scale", 1.0);
  auto_declare<double>("output_bias", 0.0);
  auto_declare<std::string>("python_module", "");
  auto_declare<std::string>("python_class", "");
  auto_declare<std::vector<double>>("demo_input", std::vector<double>{0.5, 1.0, 2.0});
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration
Ros2ControlPolicyInference::command_interface_configuration() const
{
  controller_interface::InterfaceConfiguration config;
  config.type = controller_interface::interface_configuration_type::NONE;
  return config;
}

controller_interface::InterfaceConfiguration
Ros2ControlPolicyInference::state_interface_configuration() const
{
  controller_interface::InterfaceConfiguration config;
  config.type = controller_interface::interface_configuration_type::ALL;
  return config;
}

bool Ros2ControlPolicyInference::load_backend_plugin()
{
  config_.backend_plugin = get_node()->get_parameter("backend_plugin").as_string();
  config_.model_uri = get_node()->get_parameter("model_uri").as_string();
  config_.output_size = static_cast<std::size_t>(get_node()->get_parameter("output_size").as_int());
  config_.output_scale = get_node()->get_parameter("output_scale").as_double();
  config_.output_bias = get_node()->get_parameter("output_bias").as_double();
  config_.python_module = get_node()->get_parameter("python_module").as_string();
  config_.python_class = get_node()->get_parameter("python_class").as_string();
  demo_input_ = get_node()->get_parameter("demo_input").as_double_array();

  try
  {
    backend_loader_ = std::make_unique<pluginlib::ClassLoader<InferenceBackendBase>>(
      "policy_inference_demo",
      "policy_inference_demo::InferenceBackendBase");
    auto backend_instance = backend_loader_->createUniqueInstance(config_.backend_plugin);
    backend_ = std::shared_ptr<InferenceBackendBase>(std::move(backend_instance));
  }
  catch (const pluginlib::PluginlibException & ex)
  {
    RCLCPP_ERROR(
      kLogger,
      "Failed to load backend plugin '%s': %s",
      config_.backend_plugin.c_str(),
      ex.what());
    return false;
  }

  if (!backend_->initialize(config_))
  {
    RCLCPP_ERROR(
      kLogger,
      "Backend '%s' failed to initialize.",
      config_.backend_plugin.c_str());
    backend_.reset();
    return false;
  }

  return true;
}

bool Ros2ControlPolicyInference::run_inference_once(const char * context_tag)
{
  if (!backend_)
  {
    return false;
  }

  InferenceRequest request;
  request.features = demo_input_;
  InferenceResponse response;

  if (!backend_->infer(request, response))
  {
    RCLCPP_WARN(kLogger, "[%s] Inference call failed.", context_tag);
    return false;
  }

  if (std::strcmp(context_tag, "update") == 0)
  {
    const auto & clock = *get_node()->get_clock();
    RCLCPP_INFO_THROTTLE(
      kLogger, clock, 2000, "[update] Backend '%s' produced %zu outputs.",
      response.backend_id.c_str(), response.outputs.size());
    return true;
  }

  RCLCPP_INFO(
    kLogger, "[%s] Backend '%s' produced %zu outputs.", context_tag, response.backend_id.c_str(),
    response.outputs.size());
  return true;
}

controller_interface::CallbackReturn Ros2ControlPolicyInference::on_configure(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  initialized_ = false;
  active_ = false;
  backend_.reset();
  backend_loader_.reset();

  if (!load_backend_plugin())
  {
    return controller_interface::CallbackReturn::ERROR;
  }

  initialized_ = true;
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn Ros2ControlPolicyInference::on_activate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  if (!initialized_ || !backend_)
  {
    return controller_interface::CallbackReturn::ERROR;
  }
  active_ = true;
  run_inference_once("on_activate");
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn Ros2ControlPolicyInference::on_deactivate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  active_ = false;
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::return_type Ros2ControlPolicyInference::update(
  const rclcpp::Time & /*time*/,
  const rclcpp::Duration & /*period*/)
{
  if (!initialized_ || !active_ || !backend_)
  {
    return controller_interface::return_type::ERROR;
  }
  return run_inference_once("update") ? controller_interface::return_type::OK
                                      : controller_interface::return_type::ERROR;
}

}  // namespace policy_inference_demo

PLUGINLIB_EXPORT_CLASS(
  policy_inference_demo::Ros2ControlPolicyInference,
  controller_interface::ControllerInterface)
