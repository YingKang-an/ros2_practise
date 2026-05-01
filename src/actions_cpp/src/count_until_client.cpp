#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <my_robot_interfaces/action/count_until.hpp>

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ClientGoalHandle<CountUntil>;

class CountUntilClientNode : public rclcpp::Node {
public:
  CountUntilClientNode() : Node("count_until_client") {
    count_until_client_ = rclcpp_action::create_client<CountUntil>(
        this, "count_until");  
  }

  void send_goal(int target_number, double period) {
    // 等待动作服务器
    count_until_client_->wait_for_action_server();
    // 创建目标
    auto goal = CountUntil::Goal();
    goal.target_number = target_number;
    goal.period =  period;
    // 添加结果回调
    auto options = rclcpp_action::Client<CountUntil>::SendGoalOptions();
    options.result_callback = std::bind(
        &CountUntilClientNode::goal_result_callback, this, std::placeholders::_1);
    options.goal_response_callback = std::bind(
        &CountUntilClientNode::goal_response_callback, this, std::placeholders::_1);
    options.feedback_callback = std::bind(&CountUntilClientNode::goal_feedback_callback,
        this, std::placeholders::_1, std::placeholders::_2);
    // 发送目标
    RCLCPP_INFO(this->get_logger(), "Sending a goal");
    count_until_client_->async_send_goal(goal, options);
  }

private:
  // 目标接受回调
  void goal_response_callback(const CountUntilGoalHandle::SharedPtr& goal_handle) {
    if (nullptr == goal_handle) {
      RCLCPP_INFO(this->get_logger(), "Goal got rejected");
    } else {
      RCLCPP_INFO(this->get_logger(), "Goal got accepted");
    }
  }
  // 调用结果回调
  void goal_result_callback(const CountUntilGoalHandle::WrappedResult& result) {
    auto status = result.code;
    if (rclcpp_action::ResultCode::SUCCEEDED == status) {
      RCLCPP_INFO(this->get_logger(), "Succeeded");
    } else if (rclcpp_action::ResultCode::ABORTED == status) {
      RCLCPP_WARN(this->get_logger(), "Aborted");
    } else {
      RCLCPP_INFO(this->get_logger(), " ");
    }
    int reached_number = result.result->reached_number;
    RCLCPP_INFO(this->get_logger(), "Result: %d", reached_number);
  }
  // 反馈回调
  void goal_feedback_callback(const CountUntilGoalHandle::SharedPtr & goal_handle,
      const std::shared_ptr<const CountUntil::Feedback> feedback) {
    (void) goal_handle;
    int number = feedback->current_number;
    RCLCPP_INFO(this->get_logger(), "Got feedback : %d", number);
  }
private:
  rclcpp_action::Client<CountUntil>::SharedPtr count_until_client_; 
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CountUntilClientNode>();
  node->send_goal(20, 0.8); //////////////
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}