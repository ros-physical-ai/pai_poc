# `policy_inference_standalone`

Standalone ROS 2 host package for running policy inference backends without `ros2_control`.

## What this package provides

1. `standalone_inference_demo` executable.
2. Generic launch file for backend plugin selection and backend parameter wiring.

## Dependencies

The backend contract and backend plugins are provided by `policy_inference_demo`.
This package only hosts the standalone runtime entrypoint.

## Install

Build and source the packages required to run this package's executable and launch file:

```bash
cd colcon_ws
colcon build --symlink-install --packages-up-to policy_inference_demo policy_inference_standalone
source install/setup.bash
```

For the external backend examples below, also build:

```bash
cd colcon_ws
colcon build --symlink-install --packages-up-to my_fancy_cpp_inference my_fancy_python_inference
source install/setup.bash
```

## Run

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

External C++ backend package example:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=my_fancy_cpp_inference/FancyCppBackend
```

External Python backend package example:

```bash
ros2 launch policy_inference_standalone standalone_inference_demo.launch.py \
  backend_plugin:=policy_inference_demo/PythonBackendBridge \
  python_module:=my_fancy_python_inference.fancy_python_backend \
  python_class:=FancyPythonBackend
```
