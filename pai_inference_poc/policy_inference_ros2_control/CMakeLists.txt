cmake_minimum_required(VERSION 3.16)
project(policy_inference_ros2_control LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ament_cmake REQUIRED)
find_package(controller_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(policy_inference_demo REQUIRED)
find_package(rclcpp REQUIRED)

add_library(policy_inference_ros2_control_plugin SHARED
  src/interface/ros2_control_policy_inference.cpp
)
target_include_directories(policy_inference_ros2_control_plugin PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(
  policy_inference_ros2_control_plugin
  controller_interface
  pluginlib
  policy_inference_demo
  rclcpp
)

pluginlib_export_plugin_description_file(
  controller_interface
  policy_inference_ros2_control_plugins.xml
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  FILES
    policy_inference_ros2_control_plugins.xml
    README.md
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY
    config
    launch
    urdf
  DESTINATION share/${PROJECT_NAME}
)

install(
  TARGETS policy_inference_ros2_control_plugin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_ros2_control_plugin test/test_ros2_control_plugin.cpp)
  ament_target_dependencies(test_ros2_control_plugin controller_interface pluginlib rclcpp)
  target_link_libraries(test_ros2_control_plugin policy_inference_ros2_control_plugin)
  if(TARGET test_ros2_control_plugin)
    set_tests_properties(
      test_ros2_control_plugin
      PROPERTIES ENVIRONMENT "ROS_LOG_DIR=${CMAKE_CURRENT_BINARY_DIR}/ros_test_logs"
    )
  endif()
endif()

ament_export_include_directories(include)
ament_export_dependencies(controller_interface pluginlib policy_inference_demo rclcpp)

ament_package()
