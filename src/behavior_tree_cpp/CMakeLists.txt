cmake_minimum_required(VERSION 3.8)
project(behavior_tree_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(ament_index_cpp REQUIRED)

add_executable(factory src/factory.cpp)
ament_target_dependencies(
  factory
  rclcpp
  behaviortree_cpp
)
target_include_directories(factory PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
#------------------------#
add_executable(my_first_bt src/my_first_bt.cpp src/dummy_nodes.cpp)
ament_target_dependencies(
  my_first_bt
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(my_first_bt PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_executable(my_blackboard src/my_blackboard.cpp src/blackboard_nodes.cpp)
ament_target_dependencies(
  my_blackboard
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(my_blackboard PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
  TARGETS factory my_first_bt my_blackboard
  DESTINATION lib/${PROJECT_NAME}
)
install(
  DIRECTORY trees
  DESTINATION share/${PROJECT_NAME}
)
ament_package()
