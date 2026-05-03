#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <my_robot_interfaces/action/count_until.hpp>

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ServerGoalHandle<CountUntil>; 

class CountUntilServerNode : public rclcpp::Node {
public:
  CountUntilServerNode() : Node("count_until_server") {
    // 创建可重入回调组，允许多个回调并发执行
    callback_group_ = this->create_callback_group(
        rclcpp::CallbackGroupType::Reentrant);
    // 创建 action 服务器，绑定 goal/cancel/accepted 回调
    count_until_server_ = rclcpp_action::create_server<CountUntil>(
        this, "count_until", 
        std::bind(&CountUntilServerNode::goal_callback, this,
            std::placeholders::_1, std::placeholders::_2), // 目标审核 回调
        std::bind(&CountUntilServerNode::cancel_callback, this, 
            std::placeholders::_1), // 取消请求 回调
        std::bind(&CountUntilServerNode::handle_accepted_callback, this,
            std::placeholders::_1), // 受理通过 回调
        rcl_action_server_get_default_options(), callback_group_);
  }

private:
  rclcpp_action::GoalResponse goal_callback(  // 目标审核回调
      const rclcpp_action::GoalUUID& uuid, 
      std::shared_ptr<const CountUntil::Goal> goal) {
    (void) uuid;
    RCLCPP_INFO(this->get_logger(), "Received a goal");
    // 校验目标参数，target_number 和 period 都必须大于 0
    if (0 >= goal->target_number || 0 >= goal->period) {
      RCLCPP_INFO(this->get_logger(), "Rejecting the goal");
      return rclcpp_action::GoalResponse::REJECT;
    } else {
      // 如果已有一个旧 goal 正在执行，则记录其 goal_id 用于后续中止旧 goal
      mutex_.lock();
      if (goal_handle_ && goal_handle_->is_active()) {
        preepmted_goal_id_ = goal_handle_->get_goal_id();
      }
      mutex_.unlock();
      RCLCPP_INFO(this->get_logger(), "Accepting the goal");
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
  }

  rclcpp_action::CancelResponse cancel_callback(  // 请求取消回调
      const std::shared_ptr<CountUntilGoalHandle> goal_handle) {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(), "Received cancel request");
        return rclcpp_action::CancelResponse::ACCEPT;
      }

  void handle_accepted_callback( // 受理通过回调
      const std::shared_ptr<CountUntilGoalHandle> goal_handle) {
    RCLCPP_INFO(this->get_logger(), "Executing the goal");
    execute_goal(goal_handle);
  }

  void execute_goal(const std::shared_ptr<CountUntilGoalHandle> goal_handle) {  // 执行目标回调
    mutex_.lock();
    this->goal_handle_ = goal_handle;
    mutex_.unlock();
    // 从目标获取请求参数
    int target_number = goal_handle->get_goal()->target_number;
    double period = goal_handle->get_goal()->period;
    // 执行动作循环，逐步发送反馈
    int counter = 0;
    auto feedback = std::make_shared<CountUntil::Feedback>();
    auto result = std::make_shared<CountUntil::Result>();
    rclcpp::Rate loop_rate(1.0 / period);
    for (int i = 0; i < target_number; i++) {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        // 如果当前 goal 已被新 goal 抢占，则中止旧 goal
        if (preepmted_goal_id_ == goal_handle->get_goal_id()) {
          RCLCPP_INFO(this->get_logger(), "Abort current request and accept new goal");
          result->reached_number = counter;
          goal_handle->abort(result);
          return;
        }
      }
      // 检查是否收到取消请求
      if (goal_handle->is_canceling()) {
        result->reached_number = counter;
        goal_handle->canceled(result);
        return;
      } else {
        // 执行
        counter++;
        RCLCPP_INFO(this->get_logger(), "%d", counter);
        feedback->current_number = counter;
        goal_handle->publish_feedback(feedback);
        loop_rate.sleep();
      }
    }
    // 目标完成，标记成功并返回结果
    result->reached_number = counter;
    goal_handle->succeed(result);
  }

private:
  rclcpp_action::Server<CountUntil>::SharedPtr count_until_server_;
  rclcpp::CallbackGroup::SharedPtr callback_group_;
  std::shared_ptr<CountUntilGoalHandle> goal_handle_;
  std::mutex mutex_;
  rclcpp_action::GoalUUID preepmted_goal_id_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CountUntilServerNode>();
  rclcpp::executors::MultiThreadedExecutor executer;
  executer.add_node(node);
  // 启动 executor 循环，等待并处理 action 请求
  executer.spin();
  rclcpp::shutdown();
  return 0;
}