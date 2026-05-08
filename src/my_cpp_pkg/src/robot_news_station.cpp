#include <rclcpp/rclcpp.hpp>
#include <example_interfaces/msg/string.hpp>

using namespace std::chrono_literals;

class RobotNewsStationNode : public rclcpp::Node {
public:
  RobotNewsStationNode() : Node("robot_news_station") {
    // 声明参数
    this->declare_parameter("name", "R2D2");
    this->declare_parameter("period", 0.7);
    // 创建发布器
    publisher_ = this->create_publisher<example_interfaces::msg::String>("robot_news", 10);
    // 读取初始参数
    update_params();
    // 注册参数回调
    param_cb_ = this->add_post_set_parameters_callback(
      std::bind(&RobotNewsStationNode::update_params, this)
    );
    RCLCPP_INFO(this->get_logger(), "Robot news station started!");
  }

private:
  // 统一读取参数 + 重置定时器
  void update_params() {
    robot_name_ = this->get_parameter("name").as_string();
    period_ = this->get_parameter("period").as_double();
    reset_timer(); // 改参数后必须重启定时器
  }

  void reset_timer() {
    timer_ = this->create_wall_timer(
      std::chrono::duration<double>(period_),
      [this]() { publishNews(); }
    );
  }

  void publishNews() {
    auto msg = example_interfaces::msg::String();
    msg.data = "Hi, this is " + robot_name_ + " from the robot station.";
    publisher_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "Hi, this is %s from the robot station.", robot_name_.c_str());
  }

  std::string robot_name_;
  double period_;

  rclcpp::Publisher<example_interfaces::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::node_interfaces::PostSetParametersCallbackHandle::SharedPtr param_cb_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<RobotNewsStationNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}