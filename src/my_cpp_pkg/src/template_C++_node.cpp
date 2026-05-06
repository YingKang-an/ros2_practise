#include "my_cpp_pkg/template_C++_node.hpp"

using namespace std::chrono_literals;

namespace my_cpp_pkg
{

MyNode::MyNode(const rclcpp::NodeOptions & options)
: Node("my_node", options)
{
  publisher_ = this->create_publisher<std_msgs::msg::String>("out_topic", 10);

  subscription_ = this->create_subscription<std_msgs::msg::String>(
    "in_topic", 10,
    std::bind(&MyNode::subCallback, this, std::placeholders::_1));

  timer_ = this->create_wall_timer(
    500ms, std::bind(&MyNode::timerCallback, this));

  RCLCPP_INFO(this->get_logger(), "MyNode started");
}

void MyNode::timerCallback()
{
  auto message = std_msgs::msg::String();
  message.data = "hello " + std::to_string(count_++);
  publisher_->publish(message);
  RCLCPP_INFO(this->get_logger(), "Published: '%s'", message.data.c_str());
}

void MyNode::subCallback(const std_msgs::msg::String::SharedPtr msg)
{
  RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());
}

}  // namespace my_cpp_pkg



// ---------------------------------------------
// ---------------------------------------------

#include "rclcpp/rclcpp.hpp"
#include "my_cpp_pkg/template_C++_node.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<my_cpp_pkg::MyNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}