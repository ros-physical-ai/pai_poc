# `policy_inference_ros2_control`

Thin `ros2_control` adapter package for policy inference backends.

## What this package provides

1. `policy_inference_ros2_control/Ros2ControlPolicyInference` controller plugin.
2. Demo `ros2_control` launch, controller config, and URDF files.

## Dependencies

The backend contract and backend plugins are provided by `policy_inference_core`.
This package only hosts the `ros2_control` integration layer.

## Install

Build and source the packages required to run this package's launch and plugin:

```bash
cd colcon_ws
colcon build --symlink-install --packages-up-to policy_inference_core policy_inference_ros2_control
source install/setup.bash
```

## Run

### `ros2_control` bringup

Run with the built-in C++ example backend:

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix policy_inference_ros2_control)/share/policy_inference_ros2_control/\
config/policy_inference_controllers_example_cpp.yaml"
```

Run with the built-in Python example backend:

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix policy_inference_ros2_control)/share/policy_inference_ros2_control/\
config/policy_inference_controllers_example_python.yaml"
```
