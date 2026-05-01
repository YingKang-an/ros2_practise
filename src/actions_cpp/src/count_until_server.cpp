#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <my_robot_interfaces/action/count_until.hpp>

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ServerGoalHandle<CountUntil>; 

class CountUntilServerNode : public rclcpp::Node {
public:
  CountUntilServerNode() : Node("count_until_server") {
    count_until_server_ = rclcpp_action::create_server<CountUntil>(
        this, "count_until", 
        std::bind(&CountUntilServerNode::goal_callback, this, std::placeholders::_1, std::placeholders::_2), // 目标审核回调
        std::bind(&CountUntilServerNode::cancel_callback, this, std::placeholders::_1), // 取消请求回调
        std::bind(&CountUntilServerNode::handle_accepted_callback, this, std::placeholders::_1) // 受理通过回调 
    );
  }

private:
  rclcpp_action::GoalResponse goal_callback(  // 目标审核回调
      const rclcpp_action::GoalUUID& uuid, 
      std::shared_ptr<const CountUntil::Goal> goal) {
    (void) uuid;
    RCLCPP_INFO(this->get_logger(), "Received a goal");
    if (goal->target_number <= 0) {
      RCLCPP_INFO(this->get_logger(), "Rejecting the goal");
      return rclcpp_action::GoalResponse::REJECT;
    } else {
      RCLCPP_INFO(this->get_logger(), "Accepting the goal");
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
  }

  rclcpp_action::CancelResponse cancel_callback(  // 请求取消回调
      const std::shared_ptr<CountUntilGoalHandle> goal_handle) {
        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
      }

  void handle_accepted_callback( // 受理通过回调
      const std::shared_ptr<CountUntilGoalHandle> goal_handle) {
    RCLCPP_INFO(this->get_logger(), "Executing the goal");
    execute_goal(goal_handle);
  }

  void execute_goal(const std::shared_ptr<CountUntilGoalHandle> goal_handle) {  // 执行目标回调
    // 从目标获取请求
    int target_number = goal_handle->get_goal()->target_number;
    double period = goal_handle->get_goal()->period;
    // 执行动作
    int counter = 0;
    auto feedback = std::make_shared<CountUntil::Feedback>();
    rclcpp::Rate loop_rate(1.0 / period);
    for (int i = 0; i < target_number; i++) {
      counter++;
      RCLCPP_INFO(this->get_logger(), "%d", counter);
      feedback->current_number = counter;
      goal_handle->publish_feedback(feedback);
      loop_rate.sleep();
    }
    // 设置和最终状态,返回结果
    auto result = std::make_shared<CountUntil::Result>();
    result->reached_number = counter;
    goal_handle->abort(result);
  }

private:
  rclcpp_action::Server<CountUntil>::SharedPtr count_until_server_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CountUntilServerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}