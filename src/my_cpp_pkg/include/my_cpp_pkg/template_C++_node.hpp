#ifndef MY_CPP_PKG__MY_NODE_HPP_
#define MY_CPP_PKG__MY_NODE_HPP_

#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

namespace my_cpp_pkg
{

class MyNode : public rclcpp::Node
{
public:
  explicit MyNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
  void timerCallback();
  void subCallback(const std_msgs::msg::String::SharedPtr msg);

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
  size_t count_{0};
};

}  // namespace my_cpp_pkg

#endif  // MY_CPP_PKG__MY_NODE_HPP_