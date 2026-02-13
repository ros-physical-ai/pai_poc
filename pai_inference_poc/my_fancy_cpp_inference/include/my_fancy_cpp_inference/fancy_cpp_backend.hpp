/**
 * @file fancy_cpp_backend.hpp
 * @brief Minimal C++ inference backend plugin example for `policy_inference_demo`.
 */

#pragma once

#include "policy_inference_demo/core/inference_backend_base.hpp"

#include <string>

namespace my_fancy_cpp_inference
{

/**
 * @brief Tiny backend that prints a recognizable status message during inference.
 *
 * This package demonstrates how little code is needed to add a new C++ backend
 * to the `policy_inference_demo` backend plugin layer.
 */
class FancyCppBackend : public policy_inference_demo::InferenceBackendBase
{
public:
  bool initialize(const policy_inference_demo::InferenceBackendConfig & config) override;

  bool infer(
    const policy_inference_demo::InferenceRequest & request,
    policy_inference_demo::InferenceResponse & response) override;

  std::string id() const override;

private:
  bool initialized_ = false;
  policy_inference_demo::InferenceBackendConfig config_;
  std::size_t infer_call_count_ = 0U;
};

}  // namespace my_fancy_cpp_inference

