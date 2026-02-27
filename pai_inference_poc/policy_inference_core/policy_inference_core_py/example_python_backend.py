"""Example Python inference implementation consumed by `PythonBackendBridge`."""


class ExamplePythonBackend:
    """Deterministic Python backend used by the C++ bridge plugin."""

    def __init__(self) -> None:
        self._initialized = False
        self._output_size = 3
        self._output_scale = 1.0
        self._output_bias = 0.0
        self._model_uri = ""
        self._infer_call_count = 0

    def initialize(
        self,
        model_uri: str,
        output_size: int,
        output_scale: float,
        output_bias: float,
    ) -> bool:
        self._model_uri = model_uri
        self._output_size = max(1, int(output_size))
        self._output_scale = float(output_scale)
        self._output_bias = float(output_bias)
        self._initialized = bool(model_uri)
        self._infer_call_count = 0
        print(
            "[ExamplePythonBackend] initialized "
            f"(model_uri={self._model_uri}, output_size={self._output_size})",
            flush=True,
        )
        return self._initialized

    def infer(self, features: list[float]) -> dict:
        if not self._initialized:
            raise RuntimeError("Backend is not initialized.")
        if not features:
            raise ValueError("features must not be empty")

        self._infer_call_count += 1
        if self._infer_call_count == 1 or self._infer_call_count % 100 == 0:
            print(
                "[ExamplePythonBackend] heartbeat: "
                f"infer_call_count={self._infer_call_count}",
                flush=True,
            )

        feature_values = [float(v) for v in features]
        feature_sum = float(sum(feature_values))
        feature_mean = feature_sum / float(len(feature_values))
        feature_abs_max = max(abs(v) for v in feature_values)

        outputs = [feature_sum, feature_mean, feature_abs_max]
        while len(outputs) < self._output_size:
            outputs.append(feature_values[(len(outputs) - 3) % len(feature_values)])

        scaled_outputs = [
            value * self._output_scale + self._output_bias for value in outputs[: self._output_size]
        ]

        return {"outputs": scaled_outputs, "backend_id": self.id()}

    def id(self) -> str:
        return "example_python_backend"
