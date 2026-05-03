#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <my_robot_interfaces/action/move_robot.hpp>

using MoveRobot = my_robot_interfaces::action::MoveRobot;
using MoveRobotGoalHandle = rclcpp_action::ClientGoalHandle<MoveRobot>;

class MoveRobotClientNode : public rclcpp::Node {
public:
  MoveRobotClientNode() : Node("move_robot_client") {
    // 创建 action 客户端，名称与服务器的 action 名称一致
    move_robot_client_ = rclcpp_action::create_client<MoveRobot>(
        this, "move_robot");
  }

  void send_goal(int position, int velocity) {
    if (!move_robot_client_->wait_for_action_server(std::chrono::seconds(5))) {
      RCLCPP_ERROR(this->get_logger(), "Action server not available");
      return;
    }

    auto goal_msg = MoveRobot::Goal();
    goal_msg.position = position;
    goal_msg.velocity = velocity;

    auto options = rclcpp_action::Client<MoveRobot>::SendGoalOptions();
    options.goal_response_callback = std::bind(
        &MoveRobotClientNode::goal_response_callback, this, std::placeholders::_1);
    options.feedback_callback = std::bind(
        &MoveRobotClientNode::goal_feedback_callback, this,
        std::placeholders::_1, std::placeholders::_2);
    options.result_callback = std::bind(
        &MoveRobotClientNode::goal_result_callback, this,
        std::placeholders::_1);

    RCLCPP_INFO_STREAM(this->get_logger(), "Sending goal: position=" << position
        << " velocity=" << velocity);
    move_robot_client_->async_send_goal(goal_msg, options);

    // // 如果需要取消目标，可在目标接受后启用定时器触发取消
    // timer_ = create_wall_timer(std::chrono::seconds(3),
    //    std::bind(&MoveRobotClientNode::timer_callback, this));
  }

private:

  // 取消目标的回调（当前注释掉，保留供后续启用）
  // void timer_callback() {
  //   RCLCPP_INFO(this->get_logger(),"Cancle the goal");
  //   count_until_client_->async_cancel_goal(goal_handle_);
  //   timer_->cancel();
  // }

  // 目标接受与拒绝回调
  void goal_response_callback(const MoveRobotGoalHandle::SharedPtr& goal_handle) {
    if (!goal_handle) {
      RCLCPP_INFO(this->get_logger(), "Goal got rejected");
      return;
    }
    goal_handle_ = goal_handle;
    RCLCPP_INFO(this->get_logger(), "Goal got accepted");
  }

  // 反馈回调: 接收服务器中间结果
  void goal_feedback_callback(
      const MoveRobotGoalHandle::SharedPtr &, 
      const std::shared_ptr<const MoveRobot::Feedback> feedback) {
    if (feedback) {
      RCLCPP_INFO_STREAM(this->get_logger(), "Feedback current_position=" << feedback->current_position);
    }
  }

  // 结果回调: 处理成功/中止/取消结果
  void goal_result_callback(const MoveRobotGoalHandle::WrappedResult& result) {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(this->get_logger(), "Result: SUCCEEDED");
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_WARN(this->get_logger(), "Result: ABORTED");
        break;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_INFO(this->get_logger(), "Result: CANCELED");
        break;
      default:
        RCLCPP_INFO(this->get_logger(), "Result: UNKNOWN");
        break;
    }
    if (result.result) {
      RCLCPP_INFO_STREAM(this->get_logger(), "Final position=" << result.result->position
          << " message='" << result.result->message << "'");
    }
  }

private:
  rclcpp_action::Client<MoveRobot>::SharedPtr move_robot_client_;
  MoveRobotGoalHandle::SharedPtr goal_handle_;

};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MoveRobotClientNode>();
  rclcpp::executors::MultiThreadedExecutor executer;
  executer.add_node(node);
  node->send_goal(1, 1);
  executer.spin();
  rclcpp::shutdown();
  return 0;
}