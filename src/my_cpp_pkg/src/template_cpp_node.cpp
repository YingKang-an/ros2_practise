#include <rclcpp/rclcpp.hpp>
#include <my_robot_interfaces/msg/hardwear_status.hpp>

class HardwareStatusPublisherNode : public rclcpp::Node{
  HardwareStatusPublisherNode() : Node("hardware_status_publisher") {

  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<HardwareStatusPublisherNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}