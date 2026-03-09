cmake_minimum_required(VERSION 3.16)
project(policy_inference_standalone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ament_cmake REQUIRED)
find_package(pluginlib REQUIRED)
find_package(policy_inference_core REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(standalone_inference_demo src/standalone_inference_demo.cpp)
ament_target_dependencies(standalone_inference_demo pluginlib policy_inference_core rclcpp)

install(
  FILES README.md
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(
  TARGETS standalone_inference_demo
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_package()
