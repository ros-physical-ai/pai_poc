# `policy_inference_core`

Shared policy inference contract and built-in backend plugins.

## Overview

This package owns the backend-facing contract and backend plugin implementations.
It does not host the `ros2_control` adapter or standalone executable.

For the cross-package host architecture (controller host + standalone host),
see `../README.md` section `Architecture`.

Host packages:

1. `policy_inference_ros2_control` for the controller plugin.
2. `policy_inference_standalone` for the standalone executable.


## Run

This package includes a one-shot executable for backend plugin smoke testing:
`backend_plugin_smoke`.

## Install

Build and source this package:

```bash
cd colcon_ws
colcon build --symlink-install --packages-up-to policy_inference_core
source install/setup.bash
```

### Built-in C++ backend

```bash
ros2 run policy_inference_core backend_plugin_smoke cpp
```

### Built-in Python backend

```bash
ros2 run policy_inference_core backend_plugin_smoke python
```

## Backend Plugin Layer

- Base type: `policy_inference_core::InferenceBackendBase`
- Plugin XML: `policy_inference_core/policy_inference_core_backend_plugins.xml`
- Built-in plugins:
  - `policy_inference_core/ExampleCppBackend`
  - `policy_inference_core/PythonBackendBridge`

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

Demo-only input parameter used by host packages:

1. `demo_input` (double array)

## Add a New C++ Backend

In your own package:

1. Depend on `policy_inference_core` and `pluginlib`.
2. Implement `policy_inference_core::InferenceBackendBase`.
3. Export with `PLUGINLIB_EXPORT_CLASS`.
4. Provide plugin XML with base class `policy_inference_core::InferenceBackendBase`.
5. Export the XML in CMake:
- `pluginlib_export_plugin_description_file(policy_inference_core your_backend_plugins.xml)`

Minimal shape:

```cpp
class MyCppBackend : public policy_inference_core::InferenceBackendBase
{
public:
  bool initialize(const policy_inference_core::InferenceBackendConfig & config) override;
  bool infer(
    const policy_inference_core::InferenceRequest & request,
    policy_inference_core::InferenceResponse & response) override;
  std::string id() const override { return "my_cpp_backend"; }
};
```

## Add a New Python Backend

No changes to `policy_inference_core` are required when using `PythonBackendBridge`.

Implement a Python class with:

1. `initialize(model_uri, output_size, output_scale, output_bias) -> bool`
2. `infer(features) -> dict | list[float]`
3. `id() -> str`

Then configure:

1. `backend_plugin: policy_inference_core/PythonBackendBridge`
2. `python_module: your_package.your_module`
3. `python_class: YourBackendClass`

## Notes

1. This is an architecture skeleton, not a production real-time controller.
2. Input collection and output publishing are deferred to future iterations.

## References

1. `kinematics_interface` repository [kinematics_interface](https://github.com/ros-controls/kinematics_interface)
