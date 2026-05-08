#include <rclcpp/rclcpp.hpp>
#include <my_robot_interfaces/msg/hardwear_status.hpp>

class HardwareStatusClientNode : public rclcpp::Node {
public:
  HardwareStatusClientNode() : Node("hardware_status_client") {
    subscription_ = this->create_subscription<my_robot_interfaces::msg::HardwearStatus>(
        "hardware_status", 10,
        std::bind(&HardwareStatusClientNode::on_hardware_status,
        this, std::placeholders::_1));
    RCLCPP_INFO(this->get_logger(), "Hardware status client started");
  }

private:
  void on_hardware_status(const my_robot_interfaces::msg::HardwearStatus::SharedPtr msg) const {
    RCLCPP_INFO(this->get_logger(),
        "hardware_status: temperature=%.2f, are_motor_ready=%s, debug_message='%s'",
        msg->temperature,
        msg->are_motor_ready ? "true" : "false",
        msg->debug_message.c_str());
  }

private:
  rclcpp::Subscription<my_robot_interfaces::msg::HardwearStatus>::SharedPtr subscription_;
};

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<HardwareStatusClientNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
