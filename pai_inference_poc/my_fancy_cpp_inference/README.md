# my_fancy_cpp_inference

Minimal external C++ backend plugin for `policy_inference_core`.

## Backend Plugin ID

`my_fancy_cpp_inference/FancyCppBackend`

## What it does

1. Prints an initialize message.
2. Prints a heartbeat on the first inference call (and every 100 calls).
3. Produces deterministic numeric outputs.

## Use with `policy_inference_core`

Set the backend in your controller or standalone parameters:

```yaml
policy_inference:
  ros__parameters:
    backend_plugin: my_fancy_cpp_inference/FancyCppBackend
    model_uri: fancy_cpp.onnx
    output_size: 4
    output_scale: 1.0
    output_bias: 0.0
```

