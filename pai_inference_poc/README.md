# Policy Inference PoC

This repository contains a PoC skeleton for pluggable policy inference that can run:

1. Inside `ros2_control` as a controller.
2. Outside `ros2_control` as a standalone node.

The core design mirrors dynamic interface/plugin patterns from `kinematics_interface`:

1. Keep a thin `ros2_control` adapter.
2. Keep inference logic in backend implementations behind one shared interface.
3. Select backends dynamically at runtime via plugin configuration.

## Architecture

The architecture has one shared backend contract/package and two host packages.

```text
`ros2_control` host path

[controller_manager]
        |
        | loads controller plugin
        v
[policy_inference_ros2_control/Ros2ControlPolicyInference]
        |
        | pluginlib loads `policy_inference_demo::InferenceBackendBase`
        v
[Backend Plugin]
  - policy_inference_demo/ExampleCppBackend
  - policy_inference_demo/PythonBackendBridge
  - my_fancy_cpp_inference/FancyCppBackend
  - external backend plugins


standalone host path

[policy_inference_standalone/standalone_inference_demo]
        |
        | pluginlib loads `policy_inference_demo::InferenceBackendBase`
        v
[Backend Plugin]  (same plugin layer as above)
  - policy_inference_demo/ExampleCppBackend
  - policy_inference_demo/PythonBackendBridge
  - my_fancy_cpp_inference/FancyCppBackend
  - external backend plugins
```

Plugin layers:

1. Controller layer (`ros2_control`)
- Base type: `controller_interface::ControllerInterface`
- Plugin package: `policy_inference_ros2_control`
- Plugin: `policy_inference_ros2_control/Ros2ControlPolicyInference`

2. Inference backend layer (shared)
- Base type: `policy_inference_demo::InferenceBackendBase`
- Plugin package key: `policy_inference_demo`
- Built-in plugins:
  - `policy_inference_demo/ExampleCppBackend`
  - `policy_inference_demo/PythonBackendBridge`

## Package Overview

1. `policy_inference_demo`
- Shared backend contract and built-in backend plugins.
- Details: `policy_inference_demo/README.md`

2. `policy_inference_ros2_control`
- Thin `ros2_control` adapter package.
- Details: `policy_inference_ros2_control/README.md`

3. `policy_inference_standalone`
- Standalone ROS 2 host package.
- Details: `policy_inference_standalone/README.md`

4. `my_fancy_cpp_inference`
- External C++ backend plugin example.
- Demonstrates 3rd-party extension without changing `policy_inference_demo`.
- Details: `my_fancy_cpp_inference/README.md`

5. `my_fancy_python_inference`
- External Python backend example (loaded through `PythonBackendBridge`).
- Demonstrates 3rd-party extension without changing `policy_inference_demo`.
- Details: `my_fancy_python_inference/README.md`

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

Run with the external C++ backend package example:

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix my_fancy_cpp_inference)/share/my_fancy_cpp_inference/\
config/policy_inference_controllers_my_fancy_cpp.yaml"
```

Run with the external Python backend package example:

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix my_fancy_python_inference)/share/my_fancy_python_inference/\
config/policy_inference_controllers_my_fancy_python.yaml"
```

### Standalone (without `ros2_control`)

Default run (built-in C++ example backend):

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/ExampleCppBackend
```

Built-in Python example backend:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/PythonBackendBridge \
  python_module:=policy_inference_demo_py.example_python_backend \
  python_class:=ExamplePythonBackend
```

External Python backend package example:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/PythonBackendBridge \
  python_module:=my_fancy_python_inference.fancy_python_backend \
  python_class:=FancyPythonBackend
```

External C++ backend package example:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=my_fancy_cpp_inference/FancyCppBackend
```

## Why The `my_fancy_*` Packages Exist

They intentionally model the case where `policy_inference_demo` is a separate dependency and another team wants to
add inference behavior without touching original source code.

This validates two things:

1. Backend extension is dynamic (plugin-based / parameter-based).
2. The same backend concept can be used both with and without `ros2_control`.

## Where To Start

1. Read `policy_inference_demo/README.md` for core/backend architecture.
2. Read `policy_inference_ros2_control/README.md` for controller integration and bringup.
3. Read `policy_inference_standalone/README.md` for standalone usage.
4. Read `my_fancy_cpp_inference/README.md` and `my_fancy_python_inference/README.md` for extension examples.
