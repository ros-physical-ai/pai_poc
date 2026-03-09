# Policy Inference PoC

This repository contains a PoC skeleton for pluggable policy inference that can run:

1. Inside `ros2_control` as a controller.
2. Outside `ros2_control` as a standalone node.

The core design mirrors dynamic interface/plugin patterns from `kinematics_interface`:

1. Keep a thin `ros2_control` adapter.
2. Keep inference logic in backend implementations behind one shared interface.
3. Select backends dynamically at runtime via plugin configuration.

## Architecture

The architecture has one shared backend contract/package and two host packages:
(1) ros2_control and (2) standalone.

```text
`ros2_control` host path

[controller_manager]
        |
        | loads controller plugin
        v
[policy_inference_ros2_control/Ros2ControlPolicyInference]
        |
        | pluginlib loads `policy_inference_core::InferenceBackendBase`
        v
[Backend Plugin]
  - policy_inference_core/ExampleCppBackend
  - policy_inference_core/PythonBackendBridge
  - my_fancy_cpp_inference/FancyCppBackend
  - external backend plugins


standalone host path

[policy_inference_standalone/standalone_inference_demo]
        |
        | pluginlib loads `policy_inference_core::InferenceBackendBase`
        v
[Backend Plugin]  ( >>>  same plugin layer as above  <<<)
  - policy_inference_core/ExampleCppBackend
  - policy_inference_core/PythonBackendBridge
  - my_fancy_cpp_inference/FancyCppBackend
  - external backend plugins
```

There are two plugin layers (two plugins that will be loaded)

1. Controller layer (`ros2_control`)
- Base type: `controller_interface::ControllerInterface`
- Plugin package: `policy_inference_ros2_control`
- Plugin: `policy_inference_ros2_control/Ros2ControlPolicyInference`

2. Inference backend layer (shared)
- Base type: `policy_inference_core::InferenceBackendBase`
- Plugin package key: `policy_inference_core`
- Built-in plugins:
  - `policy_inference_core/ExampleCppBackend`
  - `policy_inference_core/PythonBackendBridge`


The `ros2_control` plugin layer only gets used if we want to run the inference from inside `ros2_control`.
It's basically the usual ros2_control plugin.
The inference backend plugin is a plugin that can be used to load a different backend, for example
an `ONNX` inference backend or `MyFancyInference`. The strength of this approach is that we can add
new inference backend support without touching the original base/core package. This is demonstrated
with the `my_fancy_` packages in this demo.

## Package Overview

1. `policy_inference_core`
- Shared backend contract and built-in backend plugins.
- Details: `policy_inference_core/README.md`

2. `policy_inference_ros2_control`
- Thin `ros2_control` adapter package.
- Details: `policy_inference_ros2_control/README.md`

3. `policy_inference_standalone`
- Standalone ROS 2 host package.
- Details: `policy_inference_standalone/README.md`

4. `my_fancy_cpp_inference`
- External C++ backend plugin example.
- Demonstrates 3rd-party extension without changing `policy_inference_core`.
- Details: `my_fancy_cpp_inference/README.md`

5. `my_fancy_python_inference`
- External Python backend example (loaded through `PythonBackendBridge`).
- Demonstrates 3rd-party extension without changing `policy_inference_core`.
- Details: `my_fancy_python_inference/README.md`

## Run

Before running any command below:

```bash
cd colcon_ws
colcon build --symlink-install --packages-up-to \
  policy_inference_core policy_inference_ros2_control policy_inference_standalone \
  my_fancy_cpp_inference my_fancy_python_inference
source install/setup.bash
```

### `ros2_control` bringup

For the `ros2_control` runs below:

1. `policy_inference` controller is started.
2. `policy_inference` calls the selected backend from its `update()` loop.
3. You should see one `on_activate` inference log, then periodic `[update]` logs.
   Note: The update log is intentionally throttled to every 2 seconds in code (it's a throttled log message).
   (`policy_inference_ros2_control/src/interface/ros2_control_policy_inference.cpp`)

Run with the built-in C++ backend (`policy_inference_core/ExampleCppBackend`):

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix policy_inference_ros2_control)/share/policy_inference_ros2_control/\
config/policy_inference_controllers_example_cpp.yaml"
```

Now run with the built-in Python backend.
This runs a different backend that is written in python
(`policy_inference_core/PythonBackendBridge` + `policy_inference_core_py.example_python_backend.ExamplePythonBackend`).

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix policy_inference_ros2_control)/share/policy_inference_ros2_control/\
config/policy_inference_controllers_example_python.yaml"
```

You will see an additional "heartbeat" message that is printed from that python backend.

We can change the backend again, this time to an *external* C++ backend package 
- simulating the case that a user has added a different inference in a different package
(`my_fancy_cpp_inference/FancyCppBackend`). We are still using the same framwork to run:

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix my_fancy_cpp_inference)/share/my_fancy_cpp_inference/\
config/policy_inference_controllers_my_fancy_cpp.yaml"
```

This particular backendn also prints an additional heartbeat message.

Finally, run with external *Python* backend package (still through `PythonBackendBridge`, with
`my_fancy_python_inference.fancy_python_backend.FancyPythonBackend`):

```bash
ros2 launch policy_inference_ros2_control ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix my_fancy_python_inference)/share/my_fancy_python_inference/\
config/policy_inference_controllers_my_fancy_python.yaml"
```

This was all running the inference from within the `ros2_control` controller loop.
Next, we will see how to run the same backends outside ros2_control (just from within a regular node).

### Standalone (without `ros2_control`)

For standalone runs below:

1. `standalone_inference_demo` starts as a regular ROS 2 node (no controller manager).
2. It loads the selected backend plugin directly via `pluginlib`.
3. It prints one output line every second.

Run with built-in C++ backend:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_core/ExampleCppBackend
```

Run with built-in Python backend:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_core/PythonBackendBridge \
  python_module:=policy_inference_core_py.example_python_backend \
  python_class:=ExamplePythonBackend
```

Run with external Python backend package:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_core/PythonBackendBridge \
  python_module:=my_fancy_python_inference.fancy_python_backend \
  python_class:=FancyPythonBackend
```

Run with external C++ backend package:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=my_fancy_cpp_inference/FancyCppBackend
```

## Why The `my_fancy_*` Packages Exist

They intentionally model the case where `policy_inference_core` is a separate dependency and another team wants to
add inference behavior without touching original source code.

This validates two things:

1. Backend extension is dynamic (plugin-based / parameter-based).
2. The same backend concept can be used both with and without `ros2_control`.

## Where To Start

1. Read `policy_inference_core/README.md` for core/backend architecture.
2. Read `policy_inference_ros2_control/README.md` for controller integration and bringup.
3. Read `policy_inference_standalone/README.md` for standalone usage.
4. Read `my_fancy_cpp_inference/README.md` and `my_fancy_python_inference/README.md` for extension examples.
