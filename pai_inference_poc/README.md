# Policy Inference PoC

This repository contains a PoC skeleton for pluggable policy inference that can run:

1. Inside `ros2_control` as a controller.
2. Outside `ros2_control` as a standalone node.

The core design mirrors dynamic interface/plugin patterns from `kinematics_interface`:

1. Keep a thin `ros2_control` adapter.
2. Keep inference logic in backend implementations behind one shared interface.
3. Select backends dynamically at runtime via plugin configuration.

## Package Overview

1. `policy_inference_demo`
- Core package for the PoC.
- Provides:
    - `InferenceBackendBase` (shared backend interface)
    - `Ros2ControlPolicyInference` (`ros2_control` controller plugin)
    - `standalone_inference_demo` (non-`ros2_control` executable)
- Built-in C++ and Python example backends.
- Details: `policy_inference_demo/README.md`

> [!NOTE]
> This package is intentionally just one for now, for simplicity. It's meant to be plucked
> apart in the end, with the agnostic part (the "core") living in the open source world somewhere
> outside `ros2_control`.

2. `my_fancy_cpp_inference`
- External C++ backend plugin example.
- Demonstrates 3rd-party extension without changing `policy_inference_demo`.
- Details: `my_fancy_cpp_inference/README.md`

3. `my_fancy_python_inference`
- External Python backend example (loaded through `PythonBackendBridge`).
- Demonstrates 3rd-party extension without changing `policy_inference_demo`.
- Details: `my_fancy_python_inference/README.md`

## Why The `my_fancy_*` Packages Exist

They intentionally model the case where `policy_inference_demo` is a separate dependency and another team wants to
add inference behavior without touching original source code.

This validates two things:

1. Backend extension is dynamic (plugin-based / parameter-based).
2. The same backend concept can be used both with and without `ros2_control`.

## Where To Start

1. Read `policy_inference_demo/README.md` for architecture and launch instructions.
2. Read `my_fancy_cpp_inference/README.md` and `my_fancy_python_inference/README.md` for minimal extension examples.
