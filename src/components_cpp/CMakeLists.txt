cmake_minimum_required(VERSION 3.8)
project(components_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)

# 包含头文件
include_directories(include)

# 创建可执行文件
add_executable(manual_composition 
  src/manual_composition.cpp
  src/node1.cpp
  src/node2.cpp
)

ament_target_dependencies(manual_composition rclcpp)

install(
  TARGETS manual_composition
  DESTINATION bin/${PROJECT_NAME}
)
#-----------------------------------#

#--------创建共享库[组件.so]----------#
add_library(
  runtime_component SHARED
  src/runtime_component.cpp
)
#-------说明目标依赖哪些 ROS2 包-------#
ament_target_dependencies(
  runtime_component 
  rclcpp rclcpp_components std_msgs)
#----------注册成 ROS 组件------------#
rclcpp_components_register_nodes(
  runtime_component 
  "my_namespace::PublishNumber")
#------指定编译完成后安装输出的位置------#
install(
  TARGETS runtime_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
#---------ROS2 ament包装命令---------#
ament_package()
#-----------------------------------#
