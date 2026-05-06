#include <rclcpp/rclcpp.hpp>
#include <my_robot_interfaces/msg/hardwear_status.hpp>

class HardwareStatusPublisherNode : public rclcpp::Node{
public:
  HardwareStatusPublisherNode() : Node("hardware_status_publisher") {
    publisher_ = this->create_publisher<my_robot_interfaces::msg::HardwearStatus>(
        "hardware_status", 10);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(500),
    std::bind(&HardwareStatusPublisherNode::publishHardwareStatusPublisher, this));
  }
private:
  void publishHardwareStatusPublisher() {
    auto msg = my_robot_interfaces::msg::HardwearStatus();
    msg.temperature = 57.8;
    msg.are_motor_ready = false;
    msg.debug_message = "Motors are too hot!";
    publisher_->publish(msg);
  }
private:
  rclcpp::Publisher<my_robot_interfaces::msg::HardwearStatus>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<HardwareStatusPublisherNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}