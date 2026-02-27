"""Generic standalone launch for `standalone_inference_demo`."""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description() -> LaunchDescription:
    backend_plugin_arg = DeclareLaunchArgument(
        "backend_plugin",
        default_value="policy_inference_core/ExampleCppBackend",
        description="Backend plugin id implementing `InferenceBackendBase`.",
    )
    model_uri_arg = DeclareLaunchArgument(
        "model_uri",
        default_value="example_model.onnx",
        description="Model path/URI forwarded to the backend implementation.",
    )
    output_size_arg = DeclareLaunchArgument(
        "output_size",
        default_value="3",
        description="Output size forwarded to the backend implementation.",
    )
    output_scale_arg = DeclareLaunchArgument(
        "output_scale",
        default_value="1.0",
        description="Output scale forwarded to the backend implementation.",
    )
    output_bias_arg = DeclareLaunchArgument(
        "output_bias",
        default_value="0.0",
        description="Output bias forwarded to the backend implementation.",
    )
    python_module_arg = DeclareLaunchArgument(
        "python_module",
        default_value="",
        description="Python module path when using `PythonBackendBridge`.",
    )
    python_class_arg = DeclareLaunchArgument(
        "python_class",
        default_value="",
        description="Python class name when using `PythonBackendBridge`.",
    )

    standalone_node = Node(
        package="policy_inference_standalone",
        executable="standalone_inference_demo",
        output="screen",
        parameters=[
            {
                "backend_plugin": LaunchConfiguration("backend_plugin"),
                "model_uri": LaunchConfiguration("model_uri"),
                "output_size": ParameterValue(LaunchConfiguration("output_size"), value_type=int),
                "output_scale": ParameterValue(LaunchConfiguration("output_scale"), value_type=float),
                "output_bias": ParameterValue(LaunchConfiguration("output_bias"), value_type=float),
                "python_module": LaunchConfiguration("python_module"),
                "python_class": LaunchConfiguration("python_class"),
            }
        ],
    )

    return LaunchDescription(
        [
            backend_plugin_arg,
            model_uri_arg,
            output_size_arg,
            output_scale_arg,
            output_bias_arg,
            python_module_arg,
            python_class_arg,
            standalone_node,
        ]
    )
