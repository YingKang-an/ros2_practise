#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <my_robot_interfaces/action/move_robot.hpp>

using MoveRobot = my_robot_interfaces::action::MoveRobot;
using GoalHandle = rclcpp_action::ClientGoalHandle<MoveRobot>;

class ClientNode : public rclcpp::Node {
public:
  ClientNode() : Node("client_node") {
    client_ = rclcpp_action::create_client<MoveRobot>(this, "move_robot");
  }

  void send_goal(int64_t position, int64_t velocity) {
    client_->wait_for_action_server();
    auto goal = MoveRobot::Goal();
    goal.position = position;
    goal.velocity = velocity;

    auto options = rclcpp_action::Client<MoveRobot>::SendGoalOptions();
    options.goal_response_callback = std::bind(
        &ClientNode::goal_responce_callback, this, std::placeholders::_1);
    options.feedback_callback = std::bind(
        &ClientNode::feedback_callback, this,
        std::placeholders::_1, std::placeholders::_2);
    options.result_callback = std::bind(
        &ClientNode::goal_result_callback, this, std::placeholders::_1);

    client_->async_send_goal(goal, options);
  }

private:
  void goal_responce_callback(const GoalHandle::SharedPtr& goal_handle) {
    if (nullptr == goal_handle) {
      RCLCPP_WARN(this->get_logger(), "Goal got rejected");
    } else {
      this->goal_handle_ = goal_handle;
      RCLCPP_INFO(this->get_logger(), "Goal got accepted");
    }
  }

  void feedback_callback(const GoalHandle::SharedPtr& goal_handle,
       const std::shared_ptr<const MoveRobot::Feedback>& feedback) {
    (void) goal_handle;
    int64_t current_position = feedback->current_position;
    RCLCPP_INFO(this->get_logger(), "Got feedback : %ld", current_position);
  }

    void goal_result_callback(const GoalHandle::WrappedResult& result) {
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
  rclcpp_action::Client<MoveRobot>::SharedPtr client_;
  GoalHandle::SharedPtr goal_handle_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ClientNode>();
  rclcpp::executors::MultiThreadedExecutor executer;
  executer.add_node(node);
  node->send_goal(90, 5);
  executer.spin();
  rclcpp::shutdown();
  return 0;
}