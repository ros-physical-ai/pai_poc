"""Minimal external Python backend for `policy_inference_demo`."""


class FancyPythonBackend:
    """Simple backend used through `policy_inference_demo/PythonBackendBridge`."""

    def __init__(self) -> None:
        self._initialized = False
        self._model_uri = ""
        self._output_size = 3
        self._output_scale = 1.0
        self._output_bias = 0.0
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
        self._infer_call_count = 0
        self._initialized = bool(model_uri)
        print(
            "[FancyPythonBackend] initialized "
            f"(model_uri={self._model_uri}, output_size={self._output_size})",
            flush=True,
        )
        return self._initialized

    def infer(self, features: list[float]) -> dict:
        if not self._initialized:
            raise RuntimeError("Backend not initialized.")
        if not features:
            raise ValueError("features must not be empty")

        self._infer_call_count += 1
        if self._infer_call_count == 1 or self._infer_call_count % 100 == 0:
            print(
                "[FancyPythonBackend] heartbeat: "
                f"infer_call_count={self._infer_call_count}",
                flush=True,
            )

        values = [float(v) for v in features]
        mean = sum(values) / float(len(values))
        outputs = []
        for idx in range(self._output_size):
            outputs.append((mean + float(idx)) * self._output_scale + self._output_bias)

        return {"outputs": outputs, "backend_id": self.id()}

    def id(self) -> str:
        return "my_fancy_python_backend"

