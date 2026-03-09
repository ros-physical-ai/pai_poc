/**
 * @file fancy_cpp_backend.cpp
 * @brief Implementation of minimal external C++ backend plugin.
 */

#include "my_fancy_cpp_inference/fancy_cpp_backend.hpp"

#include <numeric>

#include "pluginlib/class_list_macros.hpp"
#include "rclcpp/rclcpp.hpp"

namespace my_fancy_cpp_inference
{

namespace
{
rclcpp::Logger kLogger = rclcpp::get_logger("my_fancy_cpp_inference.backend");
}  // namespace

bool FancyCppBackend::initialize(const policy_inference_core::InferenceBackendConfig & config)
{
  config_ = config;
  infer_call_count_ = 0U;
  initialized_ = !config_.model_uri.empty();
  RCLCPP_INFO(
    kLogger,
    "[FancyCppBackend] initialized (model_uri=%s, output_size=%zu)",
    config_.model_uri.c_str(),
    config_.output_size);
  return initialized_;
}

bool FancyCppBackend::infer(
  const policy_inference_core::InferenceRequest & request,
  policy_inference_core::InferenceResponse & response)
{
  if (!initialized_ || request.features.empty())
  {
    return false;
  }

  ++infer_call_count_;
  if (infer_call_count_ == 1U || infer_call_count_ % 100U == 0U)
  {
    RCLCPP_INFO(
      kLogger,
      "[FancyCppBackend] heartbeat infer_call_count=%zu",
      infer_call_count_);
  }

  const double sum = std::accumulate(request.features.begin(), request.features.end(), 0.0);
  response.outputs.assign(config_.output_size, 0.0);
  for (std::size_t i = 0U; i < config_.output_size; ++i)
  {
    const double value = sum + static_cast<double>(i);
    response.outputs[i] = value * config_.output_scale + config_.output_bias;
  }
  response.backend_id = id();
  return true;
}

std::string FancyCppBackend::id() const
{
  return "my_fancy_cpp_backend";
}

}  // namespace my_fancy_cpp_inference

PLUGINLIB_EXPORT_CLASS(
  my_fancy_cpp_inference::FancyCppBackend,
  policy_inference_core::InferenceBackendBase)

