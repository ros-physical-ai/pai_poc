cmake_minimum_required(VERSION 3.16)
project(my_fancy_python_inference LANGUAGES CXX)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

ament_python_install_package(my_fancy_python_inference)

install(
  FILES README.md
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
