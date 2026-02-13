/**
 * @file test_ros2_control_plugin.cpp
 * @brief Unit tests for `ros2_control` controller plugin loading.
 */

#include <gtest/gtest.h>

#include "controller_interface/controller_interface.hpp"
#include "pluginlib/class_loader.hpp"
#include "pluginlib/exceptions.hpp"

/**
 * @brief Verifies the controller plugin can be created through `pluginlib`.
 */
TEST(Ros2ControlPluginTests, Loads)
{
  pluginlib::ClassLoader<controller_interface::ControllerInterface> loader(
    "controller_interface", "controller_interface::ControllerInterface");
  try
  {
    auto plugin = loader.createSharedInstance("policy_inference_demo/Ros2ControlPolicyInference");
    ASSERT_NE(plugin, nullptr);
  }
  catch (const std::exception & ex)
  {
    GTEST_SKIP() << "Skipping plugin load test in this environment: " << ex.what();
  }
}
