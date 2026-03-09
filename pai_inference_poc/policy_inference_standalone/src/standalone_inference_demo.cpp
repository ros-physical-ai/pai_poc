/**
 * @file standalone_inference_demo.cpp
 * @brief Demo executable showing backend plugin usage outside `ros2_control`.
 */

#include "policy_inference_core/inference_backend_base.hpp"
#include "policy_inference_core/policy_types.hpp"

#include <memory>
#include <string>
#include <vector>

#include "pluginlib/class_loader.hpp"
#include "pluginlib/exceptions.hpp"
#include "rclcpp/rclcpp.hpp"

/**
 * @brief Minimal node that runs a dynamically loaded backend directly.
 */
class StandaloneInferenceNode : public rclcpp::Node
{
public:
  StandaloneInferenceNode()
  : Node("standalone_inference_demo")
  {
    declare_parameter<std::string>(
      "backend_plugin",
      "policy_inference_core/ExampleCppBackend");
    declare_parameter<std::string>("model_uri", "example_model.onnx");
    declare_parameter<int>("output_size", 3);
    declare_parameter<double>("output_scale", 1.0);
    declare_parameter<double>("output_bias", 0.0);
    declare_parameter<std::string>("python_module", "");
    declare_parameter<std::string>("python_class", "");
    declare_parameter<std::vector<double>>("demo_input", {0.5, 1.0, 2.0});

    policy_inference_core::InferenceBackendConfig config;
    config.backend_plugin = get_parameter("backend_plugin").as_string();
    config.model_uri = get_parameter("model_uri").as_string();
    config.output_size = static_cast<std::size_t>(get_parameter("output_size").as_int());
    config.output_scale = get_parameter("output_scale").as_double();
    config.output_bias = get_parameter("output_bias").as_double();
    config.python_module = get_parameter("python_module").as_string();
    config.python_class = get_parameter("python_class").as_string();

    try
    {
      backend_loader_ =
        std::make_unique<pluginlib::ClassLoader<policy_inference_core::InferenceBackendBase>>(
        "policy_inference_core",
        "policy_inference_core::InferenceBackendBase");
      auto backend = backend_loader_->createUniqueInstance(config.backend_plugin);
      backend_ = std::shared_ptr<policy_inference_core::InferenceBackendBase>(std::move(backend));
    }
    catch (const pluginlib::PluginlibException & ex)
    {
      RCLCPP_ERROR(
        get_logger(),
        "Failed to load backend plugin '%s': %s",
        config.backend_plugin.c_str(),
        ex.what());
      return;
    }

    if (!backend_ || !backend_->initialize(config))
    {
      RCLCPP_ERROR(get_logger(), "Failed to initialize backend plugin.");
      return;
    }

    timer_ = create_wall_timer(
      std::chrono::milliseconds(1000),
      [this]() { run_once(); });
  }

private:
  /**
   * @brief Run one inference call and print output.
   */
  void run_once()
  {
    policy_inference_core::InferenceRequest request;
    request.features = get_parameter("demo_input").as_double_array();

    policy_inference_core::InferenceResponse response;
    if (!backend_ || !backend_->infer(request, response))
    {
      RCLCPP_WARN(get_logger(), "Inference call failed.");
      return;
    }

    std::string output_repr = "[";
    for (std::size_t i = 0U; i < response.outputs.size(); ++i)
    {
      output_repr += std::to_string(response.outputs[i]);
      if (i + 1U < response.outputs.size())
      {
        output_repr += ", ";
      }
    }
    output_repr += "]";

    RCLCPP_INFO(
      get_logger(),
      "Standalone backend '%s' outputs=%s",
      response.backend_id.c_str(),
      output_repr.c_str());
  }

  std::shared_ptr<policy_inference_core::InferenceBackendBase> backend_;
  std::unique_ptr<pluginlib::ClassLoader<policy_inference_core::InferenceBackendBase>>
    backend_loader_;
  rclcpp::TimerBase::SharedPtr timer_;
};

/**
 * @brief Process entry point.
 */
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<StandaloneInferenceNode>());
  rclcpp::shutdown();
  return 0;
}
