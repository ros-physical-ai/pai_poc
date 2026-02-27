"""Example `ros2_control` bringup for `policy_inference_ros2_control`."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, TimerAction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    package_share = get_package_share_directory("policy_inference_ros2_control")
    urdf_path = os.path.join(package_share, "urdf", "policy_inference_core.urdf")
    default_controllers = os.path.join(
        package_share,
        "config",
        "policy_inference_controllers_example_cpp.yaml",
    )

    with open(urdf_path, "r", encoding="utf-8") as urdf_file:
        robot_description_content = urdf_file.read()

    controllers_file_arg = DeclareLaunchArgument(
        "controllers_file",
        default_value=default_controllers,
        description="Path to controller parameters yaml file.",
    )

    controllers_file = LaunchConfiguration("controllers_file")

    controller_manager_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        output="screen",
        parameters=[controllers_file],
        remappings=[("robot_description", "/robot_description")],
    )

    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[{"robot_description": robot_description_content}],
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
        ],
        output="screen",
    )

    policy_inference_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "policy_inference",
            "--controller-manager",
            "/controller_manager",
        ],
        output="screen",
    )

    delayed_spawners = TimerAction(
        period=2.0,
        actions=[joint_state_broadcaster_spawner, policy_inference_spawner],
    )

    return LaunchDescription(
        [
            controllers_file_arg,
            controller_manager_node,
            robot_state_publisher_node,
            delayed_spawners,
        ]
    )
