cmake_minimum_required(VERSION 3.8)
project(behavior_tree_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Add default BehaviorTree.CPP install prefix so CMake can find the config file.
list(APPEND CMAKE_PREFIX_PATH "/opt/BehaviorTree.CPP")
set(behaviortree_cpp_DIR "/opt/BehaviorTree.CPP/share/behaviortree_cpp/cmake" CACHE PATH "Path to behaviortree_cppConfig.cmake")
find_package(behaviortree_cpp REQUIRED)
find_package(ament_index_cpp REQUIRED)

add_executable(blackboard src/blackboard.cpp)
ament_target_dependencies(
  blackboard
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(blackboard PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(blackboard PROPERTIES INSTALL_RPATH "/opt/BehaviorTree.CPP/lib")

add_executable(my_ide_tree src/my_ide_tree.cpp)
ament_target_dependencies(
  my_ide_tree
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(my_ide_tree PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(my_ide_tree PROPERTIES INSTALL_RPATH "/opt/BehaviorTree.CPP/lib")

add_executable(template_blackboard src/template_blackboard.cpp)
ament_target_dependencies(
  template_blackboard
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(template_blackboard PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(template_blackboard PROPERTIES INSTALL_RPATH "/opt/BehaviorTree.CPP/lib")

add_executable(groot_tutorial src/groot_tutorial.cpp)
ament_target_dependencies(
  groot_tutorial
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(groot_tutorial PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(groot_tutorial PROPERTIES INSTALL_RPATH "/opt/BehaviorTree.CPP/lib")

add_executable(basic_nodes src/basic_nodes.cpp)
ament_target_dependencies(
  basic_nodes
  rclcpp
  behaviortree_cpp
  ament_index_cpp
)
target_include_directories(basic_nodes PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(basic_nodes PROPERTIES INSTALL_RPATH "/opt/BehaviorTree.CPP/lib")

install(
  TARGETS blackboard my_ide_tree template_blackboard groot_tutorial basic_nodes
  DESTINATION lib/${PROJECT_NAME}
)
install(
  DIRECTORY trees
  DESTINATION share/${PROJECT_NAME}
)
ament_package()
