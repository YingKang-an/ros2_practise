// 创建了 turtle3 充当传感器,发布tf
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <future>

#include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/spawn.hpp"

using namespace std::chrono_literals;

class PointPublisher : public rclcpp::Node {
public: PointPublisher() : Node("turtle_tf2_message_broadcaster") {
    spawner_ = this->create_client<turtlesim::srv::Spawn>("spawn");
    turtle_spawning_service_ready_ = false;
    turtle_spawned_ = false;
    turtle_pose_cansubscribe_ = false;

    timer_ = this->create_wall_timer(1s, std::bind(&PointPublisher::on_timer, this));
  }

private:
  void on_timer() {
    if (turtle_spawning_service_ready_) {
      if (turtle_spawned_) {
        turtle_pose_cansubscribe_ = true;
      } else if (result_future_.valid()) {
        if (result_future_.wait_for(0s) == std::future_status::ready) {
          auto result = result_future_.get();
          RCLCPP_INFO(this->get_logger(), "Successfully spawned %s", result->name.c_str());
          turtle_spawned_ = true;
        } else {
          RCLCPP_INFO(this->get_logger(), "Spawn is not finished");
        }
      }
    } else {
      if (spawner_->service_is_ready()) {
        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
        request->name = "turtle3";
        request->x = 4.0;
        request->y = 2.0;
        request->theta = 0.0;
        result_future_ = spawner_->async_send_request(request).future.share();
        turtle_spawning_service_ready_ = true;
      } else {
        RCLCPP_INFO(this->get_logger(), "Service is not ready");
      }
    }

    if (turtle_pose_cansubscribe_ && !subscribed_) {
      vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle3/cmd_vel", 10);
      sub_ = this->create_subscription<turtlesim::msg::Pose>(
        "turtle3/pose", 10,
        std::bind(&PointPublisher::handle_turtle_pose, this, std::placeholders::_1));
      pub_ = this->create_publisher<geometry_msgs::msg::PointStamped>("turtle3/turtle_point_stamped", 10);
      subscribed_ = true;
      RCLCPP_INFO(this->get_logger(), "Subscribed to turtle3 pose and publishing PointStamped");
    }
  }

  void handle_turtle_pose(const turtlesim::msg::Pose::SharedPtr msg) {
    if (!vel_pub_ || !pub_) {
      return;
    }

    geometry_msgs::msg::Twist vel_msg;
    vel_msg.linear.x = 1.0;
    vel_msg.angular.z = 1.0;
    vel_pub_->publish(vel_msg);

    geometry_msgs::msg::PointStamped ps;
    ps.header.stamp = this->now();
    ps.header.frame_id = "world";
    ps.point.x = msg->x;
    ps.point.y = msg->y;
    ps.point.z = 0.0;
    pub_->publish(ps);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr spawner_;
  rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture result_future_;
  bool turtle_spawning_service_ready_;
  bool turtle_spawned_;
  bool turtle_pose_cansubscribe_;
  bool subscribed_{false};
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
  rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr pub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PointPublisher>());
  rclcpp::shutdown();
  return 0;
}
