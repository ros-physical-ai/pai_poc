/**
 * @file backend_plugin_smoke.cpp
 * @brief Tiny one-shot demo executable for backend plugin loading.
 */

#include "policy_inference_core/inference_backend_base.hpp"
#include "policy_inference_core/policy_types.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "pluginlib/class_loader.hpp"
#include "pluginlib/exceptions.hpp"
#include "rclcpp/rclcpp.hpp"

/**
 * @brief Program entry point.
 *
 * Optional mode argument:
 * 1. `cpp` (default): uses `policy_inference_core/ExampleCppBackend`
 * 2. `python`: uses `policy_inference_core/PythonBackendBridge`
 */
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  std::string mode = "cpp";
  if (argc > 1)
  {
    mode = argv[1];
  }
  if (mode != "cpp" && mode != "python")
  {
    std::cerr << "Usage: backend_plugin_smoke [cpp|python]\n";
    rclcpp::shutdown();
    return 1;
  }

  policy_inference_core::InferenceBackendConfig config;
  config.backend_plugin = "policy_inference_core/ExampleCppBackend";
  config.model_uri = "example_model.onnx";
  config.output_size = 3U;
  config.output_scale = 1.0;
  config.output_bias = 0.0;
  if (mode == "python")
  {
    config.backend_plugin = "policy_inference_core/PythonBackendBridge";
    config.python_module = "policy_inference_core_py.example_python_backend";
    config.python_class = "ExamplePythonBackend";
  }

  std::unique_ptr<pluginlib::ClassLoader<policy_inference_core::InferenceBackendBase>> loader;
  std::shared_ptr<policy_inference_core::InferenceBackendBase> backend;
  try
  {
    loader = std::make_unique<pluginlib::ClassLoader<policy_inference_core::InferenceBackendBase>>(
      "policy_inference_core",
      "policy_inference_core::InferenceBackendBase");
    auto backend_instance = loader->createUniqueInstance(config.backend_plugin);
    backend = std::shared_ptr<policy_inference_core::InferenceBackendBase>(std::move(backend_instance));
  }
  catch (const pluginlib::PluginlibException & ex)
  {
    std::cerr << "Failed to load backend plugin '" << config.backend_plugin << "': " << ex.what() << "\n";
    rclcpp::shutdown();
    return 1;
  }

  if (!backend || !backend->initialize(config))
  {
    std::cerr << "Failed to initialize backend plugin '" << config.backend_plugin << "'.\n";
    rclcpp::shutdown();
    return 1;
  }

  policy_inference_core::InferenceRequest request;
  request.features = {1.0, 2.0, 3.0};
  policy_inference_core::InferenceResponse response;
  if (!backend->infer(request, response))
  {
    std::cerr << "Inference call failed.\n";
    rclcpp::shutdown();
    return 1;
  }

  std::cout << "mode=" << mode << "\nbackend_id=" << response.backend_id << "\noutputs=[";
  for (std::size_t i = 0U; i < response.outputs.size(); ++i)
  {
    std::cout << response.outputs[i];
    if (i + 1U < response.outputs.size())
    {
      std::cout << ", ";
    }
  }
  std::cout << "]\n";

  backend.reset();
  loader.reset();
  rclcpp::shutdown();
  return 0;
}
