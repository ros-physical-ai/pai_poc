/**
 * @file inference_backend_base.hpp
 * @brief Abstract backend contract for `ros2_control`-agnostic inference backends.
 */

#pragma once

#include "policy_inference_demo/core/policy_types.hpp"

#include <string>

namespace policy_inference_demo
{

/**
 * @brief Backend contract implemented by concrete backend implementations.
 */
class InferenceBackendBase
{
public:
  virtual ~InferenceBackendBase() = default;

  /**
   * @brief Configure the backend for inference.
   * @param config Backend configuration.
   * @return True when initialization succeeds.
   */
  virtual bool initialize(const InferenceBackendConfig & config) = 0;

  /**
   * @brief Run one inference step.
   * @param request Input request wrapper (kept as a struct for forward-compatible
   * API evolution beyond only a raw feature vector).
   * @param response Output wrapper. Implementations should populate `outputs`
   * and a useful `backend_id` for observability.
   * @return True on successful inference.
   */
  virtual bool infer(const InferenceRequest & request, InferenceResponse & response) = 0;

  /**
   * @brief Return backend identity string.
   * @return Stable backend identifier.
   */
  virtual std::string id() const = 0;
};

}  // namespace policy_inference_demo
