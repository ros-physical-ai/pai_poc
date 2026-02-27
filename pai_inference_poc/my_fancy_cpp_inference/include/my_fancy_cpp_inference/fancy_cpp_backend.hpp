/**
 * @file fancy_cpp_backend.hpp
 * @brief Minimal C++ inference backend plugin example for `policy_inference_core`.
 */

#pragma once

#include "policy_inference_core/inference_backend_base.hpp"

#include <string>

namespace my_fancy_cpp_inference
{

/**
 * @brief Tiny backend that prints a recognizable status message during inference.
 *
 * This package demonstrates how little code is needed to add a new C++ backend
 * to the `policy_inference_core` backend plugin layer.
 */
class FancyCppBackend : public policy_inference_core::InferenceBackendBase
{
public:
  bool initialize(const policy_inference_core::InferenceBackendConfig & config) override;

  bool infer(
    const policy_inference_core::InferenceRequest & request,
    policy_inference_core::InferenceResponse & response) override;

  std::string id() const override;

private:
  bool initialized_ = false;
  policy_inference_core::InferenceBackendConfig config_;
  std::size_t infer_call_count_ = 0U;
};

}  // namespace my_fancy_cpp_inference

