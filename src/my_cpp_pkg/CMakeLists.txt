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
find_package(my_robot_interfaces REQUIRED)

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

add_executable(hardware_status_publisher
  src/hardware_status_publisher.cpp
)
ament_target_dependencies(hardware_status_publisher
  rclcpp
  my_robot_interfaces
)

add_executable(hardware_status_subscriber
  src/hardware_status_subscriber.cpp
)

ament_target_dependencies(hardware_status_subscriber
  rclcpp
  my_robot_interfaces
)

add_executable(compute_rectangle_area_server
  src/compute_rectangle_area_server.cpp
)

ament_target_dependencies(compute_rectangle_area_server
  rclcpp
  my_robot_interfaces
)

add_executable(compute_rectangle_area_client
  src/compute_rectangle_area_client.cpp
)

ament_target_dependencies(compute_rectangle_area_client
  rclcpp
  my_robot_interfaces
)

install(TARGETS
  cpp_node
  robot_news_station
  smartphone
  hardware_status_publisher
  hardware_status_subscriber
  compute_rectangle_area_server
  compute_rectangle_area_client
  template_cpp_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
