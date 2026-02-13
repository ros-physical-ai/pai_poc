/**
 * @file example_cpp_backend.hpp
 * @brief Deterministic C++ example backend used for architecture discussions.
 */

#pragma once

#include "policy_inference_demo/core/inference_backend_base.hpp"

namespace policy_inference_demo
{

/**
 * @brief A deterministic placeholder backend for C++ plugin integration.
 *
 * This backend does not load or execute a real model. It provides predictable output
 * so integration architecture can be discussed and tested first.
 */
class ExampleCppBackend : public InferenceBackendBase
{
public:
  bool initialize(const InferenceBackendConfig & config) override;
  bool infer(const InferenceRequest & request, InferenceResponse & response) override;
  std::string id() const override;

private:
  bool initialized_ = false;
  InferenceBackendConfig config_;
};

}  // namespace policy_inference_demo
