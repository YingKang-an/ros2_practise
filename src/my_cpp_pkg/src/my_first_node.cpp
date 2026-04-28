#include <rclcpp/rclcpp.hpp>

class MyNode : public rclcpp::Node {
public:
  MyNode() : Node("cpp_test") {
    RCLCPP_INFO(this->get_logger(), "Hello world!");
    timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&MyNode::time_callback, this));
  }
private:
  rclcpp::TimerBase::SharedPtr timer_;
  size_t counter_ = 0;

  void time_callback() {
    RCLCPP_INFO(this->get_logger(), "Hello %ld", counter_);
    counter_ += 1;
  }
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MyNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}