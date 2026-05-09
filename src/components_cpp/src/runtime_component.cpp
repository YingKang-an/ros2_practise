#include <rclcpp/rclcpp.hpp>
#include "components_cpp/runtime_component.hpp"

namespace my_namespace
{

PublishNumber::PublishNumber(const rclcpp::NodeOptions& options)
    : Node("publish_number_node", options), counter_(1) {
  this->get_node_base_interface();
  callback_group_ = this->create_callback_group(
      rclcpp::CallbackGroupType::Reentrant);
  publisher_ = this->create_publisher<std_msgs::msg::Int32>("number", 10);
  timer_ = this->create_wall_timer(
      std::chrono::seconds(1),
      std::bind(&PublishNumber::timer_callback, this),
      callback_group_
  );
  RCLCPP_INFO(this->get_logger(), "PublishNumber started");
}

void PublishNumber::timer_callback() {
  auto msg = std_msgs::msg::Int32();
  msg.data = counter_;
  publisher_->publish(msg);
  RCLCPP_INFO(this->get_logger(), "Publishing: %d", msg.data);
  ++counter_;
}

} // namespace my_namespace

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(my_namespace::PublishNumber)

// $ ~/Desktop/ros_practise$ ros2 component types
// components_cpp
//   my_namespace::PublishNumber

// $ ~/Desktop/ros_practise$ ros2 component load /ComponentManager components_cpp my_namespace::PublishNumber 
// Loaded component 1 into '/ComponentManager' container node as '/publish_number_node'