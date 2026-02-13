/**
 * @file example_cpp_backend.cpp
 * @brief Implementation of deterministic C++ example backend.
 */

#include "policy_inference_demo/core/example_cpp_backend.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

#include "pluginlib/class_list_macros.hpp"

namespace policy_inference_demo
{

bool ExampleCppBackend::initialize(const InferenceBackendConfig & config)
{
  config_ = config;
  initialized_ = config_.output_size > 0U;
  return initialized_;
}

bool ExampleCppBackend::infer(const InferenceRequest & request, InferenceResponse & response)
{
  if (!initialized_ || request.features.empty())
  {
    return false;
  }

  response.outputs.assign(config_.output_size, 0.0);
  response.backend_id = id();

  const double sum = std::accumulate(request.features.begin(), request.features.end(), 0.0);
  const double mean = sum / static_cast<double>(request.features.size());
  const auto max_it = std::max_element(request.features.begin(), request.features.end());
  const double abs_max = std::abs(*max_it);

  response.outputs[0] = sum * config_.output_scale + config_.output_bias;
  if (config_.output_size > 1U)
  {
    response.outputs[1] = mean * config_.output_scale + config_.output_bias;
  }
  if (config_.output_size > 2U)
  {
    response.outputs[2] = abs_max * config_.output_scale + config_.output_bias;
  }
  for (std::size_t idx = 3U; idx < config_.output_size; ++idx)
  {
    const double src = request.features[idx % request.features.size()];
    response.outputs[idx] = src * config_.output_scale + config_.output_bias;
  }

  return true;
}

std::string ExampleCppBackend::id() const
{
  return "example_cpp_backend";
}

}  // namespace policy_inference_demo

PLUGINLIB_EXPORT_CLASS(
  policy_inference_demo::ExampleCppBackend,
  policy_inference_demo::InferenceBackendBase)
