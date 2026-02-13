/**
 * @file python_backend_bridge.hpp
 * @brief Backend plugin that bridges to a Python class implementation.
 */

#pragma once

#include "policy_inference_demo/core/inference_backend_base.hpp"

#include <memory>

namespace policy_inference_demo
{

/**
 * @brief `InferenceBackendBase` implementation that forwards calls to a Python object.
 *
 * The loaded Python class must implement:
 * 1. `initialize(model_uri: str, output_size: int, output_scale: float, output_bias: float) -> bool`
 * 2. `infer(features: list[float]) -> dict | list[float]`
 *
 * If `infer` returns a dict, this bridge expects key `outputs` and optionally `backend_id`.
 */
class PythonBackendBridge : public InferenceBackendBase
{
public:
  PythonBackendBridge();
  ~PythonBackendBridge() override;

  bool initialize(const InferenceBackendConfig & config) override;
  bool infer(const InferenceRequest & request, InferenceResponse & response) override;
  std::string id() const override;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
  bool initialized_ = false;
  InferenceBackendConfig config_;
  std::string backend_id_ = "python_backend_bridge";
};

}  // namespace policy_inference_demo

