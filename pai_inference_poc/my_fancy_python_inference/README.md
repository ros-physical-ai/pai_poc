# my_fancy_python_inference

Minimal external Python inference implementation for `policy_inference_demo`.

This package is consumed through the existing C++ bridge backend plugin:
`policy_inference_demo/PythonBackendBridge`.

## Python Class

`my_fancy_python_inference.fancy_python_backend.FancyPythonBackend`

## What it does

1. Prints an initialize message.
2. Prints a heartbeat on the first inference call (and every 100 calls).
3. Returns deterministic numeric outputs.

## Use with `policy_inference_demo`

Set:

```yaml
policy_inference:
  ros__parameters:
    backend_plugin: policy_inference_demo/PythonBackendBridge
    python_module: my_fancy_python_inference.fancy_python_backend
    python_class: FancyPythonBackend
    model_uri: fancy_python.onnx
    output_size: 4
    output_scale: 1.0
    output_bias: 0.0
```

