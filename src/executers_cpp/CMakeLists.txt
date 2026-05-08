cmake_minimum_required(VERSION 3.8)
project(executers_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

include_directories(include)

add_executable(single_threaded_executor
  src/single_threaded_executor
)

ament_target_dependencies(single_threaded_executor rclcpp)

install(TARGETS single_threaded_executor
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
