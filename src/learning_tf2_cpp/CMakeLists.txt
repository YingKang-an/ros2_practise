cmake_minimum_required(VERSION 3.8)
project(learning_tf2_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(turtlesim REQUIRED)

# Executables
add_executable(static_turtle_tf2_broadcaster src/static_turtle_tf2_broadcaster.cpp)
add_executable(my_static_tf2_broadcaster src/my_static_tf2_broadcaster.cpp)
add_executable(my_dynamic_tf2_broadcaster src/my_dynamic_tf2_broadcaster.cpp)
add_executable(turtle_tf2_broadcaster src/turtle_tf2_broadcaster.cpp)
add_executable(turtle_tf2_listener src/turtle_tf2_listener.cpp)
add_executable(my_tf2_listener src/my_tf2_listener.cpp)

# Dependencies
ament_target_dependencies(static_turtle_tf2_broadcaster
  geometry_msgs
  rclcpp
  tf2
  tf2_ros
)
ament_target_dependencies(my_static_tf2_broadcaster
  geometry_msgs
  rclcpp
  tf2
  tf2_ros
)
ament_target_dependencies(my_dynamic_tf2_broadcaster
  geometry_msgs
  rclcpp
  tf2
  tf2_ros
)
ament_target_dependencies(turtle_tf2_broadcaster
  geometry_msgs
  rclcpp
  tf2
  tf2_ros
  turtlesim
)
ament_target_dependencies(turtle_tf2_listener
  geometry_msgs
  rclcpp
  tf2
  tf2_ros
  turtlesim
)
ament_target_dependencies(my_tf2_listener
  geometry_msgs
  rclcpp
  tf2
  tf2_ros
)

# Install all executables
install(TARGETS
  static_turtle_tf2_broadcaster
  my_static_tf2_broadcaster
  my_dynamic_tf2_broadcaster
  turtle_tf2_broadcaster
  turtle_tf2_listener
  my_tf2_listener
  DESTINATION lib/${PROJECT_NAME}
)

# Install launch files
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()