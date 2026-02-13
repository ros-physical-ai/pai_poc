cmake_minimum_required(VERSION 3.16)
project(my_fancy_cpp_inference LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ament_cmake REQUIRED)
find_package(pluginlib REQUIRED)
find_package(policy_inference_demo REQUIRED)
find_package(rclcpp REQUIRED)

add_library(my_fancy_cpp_inference_plugin SHARED
  src/fancy_cpp_backend.cpp
)
target_include_directories(my_fancy_cpp_inference_plugin PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(my_fancy_cpp_inference_plugin pluginlib policy_inference_demo rclcpp)

pluginlib_export_plugin_description_file(
  policy_inference_demo
  my_fancy_cpp_inference_plugins.xml
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  FILES
    my_fancy_cpp_inference_plugins.xml
    README.md
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

install(
  TARGETS my_fancy_cpp_inference_plugin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_export_include_directories(include)
ament_export_dependencies(pluginlib policy_inference_demo rclcpp)

ament_package()
