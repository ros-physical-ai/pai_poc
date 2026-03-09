/**
 * @file test_inference_core.cpp
 * @brief Unit tests for backend implementations.
 */

#include "policy_inference_core/example_cpp_backend.hpp"

#include <gtest/gtest.h>

/**
 * @brief Validates that the C++ example backend returns deterministic outputs.
 */
TEST(ExampleCppBackendTests, ConfiguresAndInfers)
{
  policy_inference_core::ExampleCppBackend backend;

  policy_inference_core::InferenceBackendConfig config;
  config.backend_plugin = "policy_inference_core/ExampleCppBackend";
  config.model_uri = "example_cpp_model.onnx";
  config.output_size = 4U;
  config.output_scale = 1.0;
  config.output_bias = 0.0;
  ASSERT_TRUE(backend.initialize(config));

  policy_inference_core::InferenceRequest request;
  request.features = {1.0, 2.0, 3.0};
  policy_inference_core::InferenceResponse response;

  ASSERT_TRUE(backend.infer(request, response));
  ASSERT_EQ(response.backend_id, "example_cpp_backend");
  ASSERT_EQ(response.outputs.size(), 4U);
  EXPECT_DOUBLE_EQ(response.outputs[0], 6.0);
  EXPECT_DOUBLE_EQ(response.outputs[1], 2.0);
  EXPECT_DOUBLE_EQ(response.outputs[2], 3.0);
}
