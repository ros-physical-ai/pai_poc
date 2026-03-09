/**
 * @file policy_types.hpp
 * @brief Shared data types for `ros2_control`-agnostic policy inference core code.
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace policy_inference_core
{

/**
 * @brief Inference request container.
 *
 * This currently wraps one numeric feature vector, but it intentionally exists
 * as a named request type so the plugin contract can evolve without changing
 * function signatures across all backend implementations.
 *
 * Future extensions can add metadata such as timestamps, frame ids, tensor
 * names, or quality flags while preserving the same infer() API.
 */
struct InferenceRequest
{
  /// Flattened input features used by this PoC skeleton.
  std::vector<double> features;
};

/**
 * @brief Inference response container.
 *
 * Similar to InferenceRequest, this wrapper keeps the backend API stable while
 * allowing future response metadata to be added without breaking interface contracts.
 */
struct InferenceResponse
{
  /// Flattened output vector produced by the backend.
  std::vector<double> outputs;
  /**
   * @brief Runtime identity of the backend that produced this response.
   *
   * In this PoC, this is mainly used for logging and test visibility. It is
   * especially useful when using bridge-style plugins (for example Python),
   * where one plugin id can dispatch to different concrete backend classes.
   */
  std::string backend_id;
};

/**
 * @brief Backend configuration used by core and adapters.
 */
struct InferenceBackendConfig
{
  std::string backend_plugin = "policy_inference_core/ExampleCppBackend";
  std::string model_uri = "example_model.onnx";
  std::size_t output_size = 3U;
  double output_scale = 1.0;
  double output_bias = 0.0;
  std::string python_module = "";
  std::string python_class = "";
};

}  // namespace policy_inference_core
