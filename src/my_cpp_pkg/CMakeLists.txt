cmake_minimum_required(VERSION 3.8)
project(my_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

# 添加头文件搜索路径，编译器会在 include 目录里查找头文件
include_directories(include)

# 定义可执行文件 cpp_node
add_executable(cpp_node
  src/my_first_node.cpp
)

# 将 cpp_node 与 ROS 2 rclcpp 库关联，确保链接时能使用 rclcpp 功能
ament_target_dependencies(cpp_node
  rclcpp
)

add_executable(robot_news_station
  src/robot_news_station.cpp
)
ament_target_dependencies(robot_news_station
  rclcpp
  example_interfaces
)

add_executable(smartphone
  src/smartphone.cpp
)
ament_target_dependencies(smartphone
  rclcpp
  example_interfaces
)

add_executable(template_cpp_node
  src/template_C++_node.cpp
)
ament_target_dependencies(template_cpp_node
  rclcpp
  std_msgs
)

install(TARGETS
  cpp_node
  robot_news_station
  smartphone
  template_cpp_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
