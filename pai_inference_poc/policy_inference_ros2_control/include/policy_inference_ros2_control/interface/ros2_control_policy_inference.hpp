/**
 * @file ros2_control_policy_inference.hpp
 * @brief Thin `ros2_control` controller that delegates inference to a backend plugin.
 *
 * References:
 * 1. `https://github.com/ros-controls/kinematics_interface`
 * 2. `kinematics_interface_kdl/include/kinematics_interface_kdl/kinematics_interface_kdl.hpp`
 * 3. `kinematics_interface_kdl/kinematics_interface_kdl.xml`
 */

#pragma once

#include "policy_inference_demo/core/inference_backend_base.hpp"

#include <memory>
#include <vector>

#include "controller_interface/controller_interface.hpp"
#include "pluginlib/class_loader.hpp"

namespace policy_inference_ros2_control
{

/**
 * @brief `kinematics_interface`-inspired adapter for `ros2_control` contexts.
 *
 * Pattern references:
 * 1. Interface extension pattern:
 *    Repo: `https://github.com/ros-controls/kinematics_interface`
 *    Path: `kinematics_interface/include/kinematics_interface/kinematics_interface.hpp`
 * 2. Plugin implementation pattern:
 *    Repo: `https://github.com/ros-controls/kinematics_interface`
 *    Path: `kinematics_interface_kdl/include/kinematics_interface_kdl/kinematics_interface_kdl.hpp`
 *
 * This class demonstrates how `ros2_control`-facing code can call dynamically loaded
 * backend logic instead of embedding model-specific code directly.
 */
class Ros2ControlPolicyInference : public controller_interface::ControllerInterface
{
public:
  controller_interface::CallbackReturn on_init() override;
  controller_interface::InterfaceConfiguration command_interface_configuration() const override;
  controller_interface::InterfaceConfiguration state_interface_configuration() const override;
  controller_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;
  controller_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;
  controller_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;
  controller_interface::return_type update(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

private:
  bool load_backend_plugin();
  bool run_inference_once(const char * context_tag);

  bool initialized_ = false;
  bool active_ = false;
  policy_inference_demo::InferenceBackendConfig config_;
  std::vector<double> demo_input_;
  std::unique_ptr<pluginlib::ClassLoader<policy_inference_demo::InferenceBackendBase>> backend_loader_;
  std::shared_ptr<policy_inference_demo::InferenceBackendBase> backend_;
};

}  // namespace policy_inference_ros2_control
