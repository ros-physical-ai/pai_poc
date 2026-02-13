# `policy_inference_demo`

Draft skeleton for policy inference integration with `ros2_control`, inspired by dynamic plugin usage patterns from
`kinematics_interface`.

## Overview

This package is structured around one `ros2_control`-agnostic backend interface:

1. `InferenceBackendBase`
- Holds inference logic.
- Implemented by concrete backends, for example:
    - `policy_inference_demo/ExampleCppBackend`
    - `policy_inference_demo/PythonBackendBridge`

2. `Ros2ControlPolicyInference`
- Thin `ros2_control` controller plugin.
- Dynamically loads one `InferenceBackendBase` implementation and calls it from `update()`.

3. `standalone_inference_demo`
- Non-`ros2_control` executable.
- Dynamically loads the same `InferenceBackendBase` plugins as the controller.

## Architecture

```text
`ros2_control` path

[controller_manager]
        |
        | loads controller plugin
        v
[Ros2ControlPolicyInference : controller_interface::ControllerInterface]
        |
        | pluginlib loads `InferenceBackendBase`
        v
[Backend Plugin]
  - policy_inference_demo/ExampleCppBackend
  - policy_inference_demo/PythonBackendBridge
  - any external package backend


standalone path

[standalone_inference_demo]
        |
        | pluginlib loads `InferenceBackendBase`
        v
[Backend Plugin]  (same plugins as above)
```

## Plugin Layers

There are two plugin layers:

1. Controller layer (`ros2_control`)
- Base type: `controller_interface::ControllerInterface`
- Loaded by: `controller_manager`
- XML file: `policy_inference_demo/policy_inference_demo_plugins.xml`
- Plugin: `policy_inference_demo/Ros2ControlPolicyInference`

2. Inference backend layer
- Base type: `policy_inference_demo::InferenceBackendBase`
- Loaded by:
    - `Ros2ControlPolicyInference`
    - `standalone_inference_demo`
- XML file: `policy_inference_demo/policy_inference_demo_backend_plugins.xml`
- Built-in plugins:
    - `policy_inference_demo/ExampleCppBackend`
    - `policy_inference_demo/PythonBackendBridge`

## Backend Parameters

Shared:

1. `backend_plugin` (string)
2. `model_uri` (string)
3. `output_size` (int)
4. `output_scale` (double)
5. `output_bias` (double)

Python bridge specific:

1. `python_module` (string)
2. `python_class` (string)

Demo-only input parameter used by the skeleton:

1. `demo_input` (double array)

## Built-In Examples

1. C++ example backend
- Plugin id: `policy_inference_demo/ExampleCppBackend`
- Class: `policy_inference_demo::ExampleCppBackend`
- File: `policy_inference_demo/src/core/example_cpp_backend.cpp`

2. Python example backend
- Bridge plugin id: `policy_inference_demo/PythonBackendBridge`
- `ExamplePythonBackend`:
    - File: `policy_inference_demo/policy_inference_demo_py/example_python_backend.py`
    - Python module/class: `policy_inference_demo_py.example_python_backend`

## Run

### `ros2_control` bringup

Run with the built-in C++ example backend:

```bash
ros2 launch policy_inference_demo ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix policy_inference_demo)/share/policy_inference_demo/\
config/policy_inference_controllers_example_cpp.yaml"
```

Run with the built-in Python example backend:

```bash
ros2 launch policy_inference_demo ros2_control_inference_demo.launch.py \
  controllers_file:="$(ros2 pkg prefix policy_inference_demo)/share/policy_inference_demo/\
config/policy_inference_controllers_example_python.yaml"
```

### Standalone (without `ros2_control`)

Default run (built-in C++ example backend):

```bash
ros2 launch policy_inference_demo standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/ExampleCppBackend 
```

Built-in Python example backend:

```bash
ros2 launch policy_inference_demo standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/PythonBackendBridge \
  python_module:=policy_inference_demo_py.example_python_backend \
  python_class:=ExamplePythonBackend
```

External Python backend package example:

```bash
ros2 launch policy_inference_demo standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/PythonBackendBridge \
  python_module:=my_fancy_python_inference.fancy_python_backend \
  python_class:=FancyPythonBackend
```

External C++ backend package example:

```bash
ros2 launch policy_inference_demo standalone_inference_demo.launch.py \
  backend_plugin:=my_fancy_cpp_inference/FancyCppBackend
```

## Add a New C++ Backend

In your own package:

1. Depend on `policy_inference_demo` and `pluginlib`.
2. Implement `policy_inference_demo::InferenceBackendBase`.
3. Export with `PLUGINLIB_EXPORT_CLASS`.
4. Provide plugin XML with base class `policy_inference_demo::InferenceBackendBase`.
5. Export the XML in CMake:
- `pluginlib_export_plugin_description_file(policy_inference_demo your_backend_plugins.xml)`

Minimal shape:

```cpp
class MyCppBackend : public policy_inference_demo::InferenceBackendBase
{
public:
  bool initialize(const policy_inference_demo::InferenceBackendConfig & config) override;
  bool infer(
    const policy_inference_demo::InferenceRequest & request,
    policy_inference_demo::InferenceResponse & response) override;
  std::string id() const override { return "my_cpp_backend"; }
};
```

Concrete example package in this workspace:

1. `my_fancy_cpp_inference`
2. Plugin id: `my_fancy_cpp_inference/FancyCppBackend`

## Add a New Python Backend

No changes to `policy_inference_demo` are required when using `PythonBackendBridge`.

Implement a Python class with:

1. `initialize(model_uri, output_size, output_scale, output_bias) -> bool`
2. `infer(features) -> dict | list[float]`
3. `id() -> str`

Then configure:

1. `backend_plugin: policy_inference_demo/PythonBackendBridge`
2. `python_module: your_package.your_module`
3. `python_class: YourBackendClass`

Concrete example package in this workspace:

1. `my_fancy_python_inference`
2. Class:
- `my_fancy_python_inference.fancy_python_backend.FancyPythonBackend`

## Notes

1. This is an architecture skeleton, not a production real-time controller.
2. Input collection and output publishing are deferred to future iterations.

## References

1. `kinematics_interface` repository [kinematics_interface](https://github.com/ros-controls/kinematics_interface)
