#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>

namespace my_namespace
{

class PublishNumber : public rclcpp::Node {
public:
  PublishNumber(const rclcpp::NodeOptions& options);

private:
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int32_t counter_;
  rclcpp::CallbackGroup::SharedPtr callback_group_;

private:
  void timer_callback();
};

} // namespace my_namespace