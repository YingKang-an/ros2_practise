#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <my_robot_interfaces/action/move_robot.hpp>

using MoveRobot = my_robot_interfaces::action::MoveRobot;
using GoalHandle = rclcpp_action::ServerGoalHandle<MoveRobot>;

class ServerNode : public rclcpp::Node {
public:
  ServerNode() : Node("move_robot_node"), robot_position_(0) {
    callback_group_ = this->create_callback_group(
        rclcpp::CallbackGroupType::Reentrant);
    serve_ = rclcpp_action::create_server<MoveRobot>(
        this, "move_robot",
        std::bind(&ServerNode::goal_callback, this,
            std::placeholders::_1, std::placeholders::_2),
        std::bind(&ServerNode::cancel_callback, this,
            std::placeholders::_1),
        std::bind(&ServerNode::accept_callback, this,
            std::placeholders::_1),
        rcl_action_server_get_default_options(),
        callback_group_
    );
  }

private:
  rclcpp_action::GoalResponse goal_callback(
      const rclcpp_action::GoalUUID uuid,
      std::shared_ptr<const MoveRobot::Goal> goal) {
    (void) uuid;
    RCLCPP_INFO(this->get_logger(), "Received a goal");
    if (0 >= goal->position || 0 >= goal->velocity) {
      RCLCPP_ERROR(this->get_logger(), "Regecting the goal");
      return rclcpp_action::GoalResponse::REJECT;
    } else {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (goal_handle_ && goal_handle_->is_active()) {
          preepmted_goal_id_ = goal_handle_->get_goal_id();
        }
      }
      RCLCPP_INFO(this->get_logger(), "Accepting the goal");
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
  }

  rclcpp_action::CancelResponse cancel_callback(
      const std::shared_ptr<GoalHandle> goal_handle) {
    (void) goal_handle;
    RCLCPP_INFO(this->get_logger(), "Received cancel request");
    return rclcpp_action::CancelResponse::ACCEPT;
  }
  
  void accept_callback(const std::shared_ptr<GoalHandle> goal_handle) {
    execute_callback(goal_handle);
  }

  void execute_callback(const std::shared_ptr<GoalHandle> goal_handle) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      this->goal_handle_ = goal_handle;
    }
    int goal_position = goal_handle->get_goal()->position;
    int velocity = goal_handle->get_goal()->velocity;

    auto feedback = std::make_shared<MoveRobot::Feedback>();
    auto result = std::make_shared<MoveRobot::Result>();

    rclcpp::Rate loop_rate(1.0);
    while(rclcpp::ok()) {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (goal_handle->get_goal_id() == this->preepmted_goal_id_) {
          result->position = robot_position_;
          result->message = "Preempted by another goal!";
          goal_handle->abort(result);
          return;
        }
      }
      if (goal_handle->is_canceling()) {
        result->position = robot_position_;
        if (goal_position == robot_position_) {
          result->message = "Succeed";
          goal_handle->succeed(result);
        } else {
          result->message = "Canceled";
          goal_handle->canceled(result);
        }
        return;
      }

      int diff = goal_position - robot_position_;
      if (0 == diff) {
        result->position = robot_position_;
        result->message = "Success";
        goal_handle->succeed(result);
        return;
      } else if (diff > 0) {
        if (diff < velocity) {
          robot_position_ += diff;
        } else {
          robot_position_ += velocity;
        }
      } else if (diff < 0) {
        if (abs(diff) < velocity) {
          robot_position_ -= abs(diff);
        } else {
          robot_position_ -= velocity;
        }
      }

      RCLCPP_INFO(this->get_logger(), "Robot position : %d", robot_position_);

      feedback->current_position = robot_position_;
      goal_handle->publish_feedback(feedback);

      loop_rate.sleep();
    }
  }

private:
  rclcpp_action::Server<MoveRobot>::SharedPtr serve_;
  rclcpp::CallbackGroup::SharedPtr callback_group_;
  std::mutex mutex_;
  rclcpp_action::GoalUUID preepmted_goal_id_;
  std::shared_ptr<GoalHandle> goal_handle_;

  int robot_position_;
};


int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ServerNode>();
  rclcpp::executors::MultiThreadedExecutor executer;
  executer.add_node(node);
  executer.spin();
  rclcpp::shutdown();
  return 0;
}