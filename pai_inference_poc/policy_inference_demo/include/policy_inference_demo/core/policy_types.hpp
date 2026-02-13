/**
 * @file policy_types.hpp
 * @brief Shared data types for `ros2_control`-agnostic policy inference core code.
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace policy_inference_demo
{

/**
 * @brief Inference request container.
 */
struct InferenceRequest
{
  std::vector<double> features;
};

/**
 * @brief Inference response container.
 */
struct InferenceResponse
{
  std::vector<double> outputs;
  std::string backend_id;
};

/**
 * @brief Backend configuration used by core and adapters.
 */
struct InferenceBackendConfig
{
  std::string backend_plugin = "policy_inference_demo/ExampleCppBackend";
  std::string model_uri = "example_model.onnx";
  std::size_t output_size = 3U;
  double output_scale = 1.0;
  double output_bias = 0.0;
  std::string python_module = "";
  std::string python_class = "";
};

}  // namespace policy_inference_demo
