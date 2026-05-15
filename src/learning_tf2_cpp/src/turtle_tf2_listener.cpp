#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
// 坐标变换消息
#include "geometry_msgs/msg/transform_stamped.hpp"
// 速度消息
#include "geometry_msgs/msg/twist.hpp"
#include "tf2/exceptions.h"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
// 生成乌龟的服务
#include "turtlesim/srv/spawn.hpp"

using namespace std::chrono_literals;

class FrameListener : public rclcpp::Node {
public:
  FrameListener() : Node("tf2_listener"),
      turtle_spawning_service_ready_(false),
      turtle_spawned_(false) {
    // 声明参数：要跟随的目标坐标系(默认是 turtle1)
    target_frame_ = this->declare_parameter<std::string>(
        "target_frame", "turtle1");
    // 创建 TF 缓冲区 (TF 数据是时间相关的)
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    // 创建 TF 监听器(自动订阅 /tf 和 /tf_static)
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    //  创建客户端：用于调用服务生成第二只乌龟 turtle2
    spawner_ = this->create_client<turtlesim::srv::Spawn>("spawn");
    // 创建发布者：给 turtle2 发速度指令
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "turtle2/cmd_vel", 1);
    // Call on_timer function every second
    timer_ = this->create_wall_timer(
      1s, [this]() {return this->on_timer();});
  }

private:
  void on_timer() {
    // Store frame names in variables that will be used to
    // compute transformations
    std::string fromFrameRel = target_frame_.c_str();
    std::string toFrameRel = "turtle2";

    if (turtle_spawning_service_ready_) {
      if (turtle_spawned_) {
        geometry_msgs::msg::TransformStamped t;

        // Look up for the transformation between target_frame and turtle2 frames
        // and send velocity commands for turtle2 to reach target_frame
        try {
          // 在目标坐标系 toFrameRel 下，fromFrameRel 的位置和朝向是多少
          t = tf_buffer_->lookupTransform(toFrameRel, fromFrameRel,
              tf2::TimePointZero);
          RCLCPP_INFO_STREAM(this->get_logger(), 
              "frame_id: " << t.header.frame_id);
          RCLCPP_INFO_STREAM(this->get_logger(),
              "stamp: " << t.header.stamp.sec << "." 
              << std::setw(9) << std::setfill('0') << t.header.stamp.nanosec);
          RCLCPP_INFO_STREAM(this->get_logger(),
              "child_frame_id: " << t.child_frame_id);
          RCLCPP_INFO_STREAM(this->get_logger(),
              "translation: (" << t.transform.translation.x << ", "
              << t.transform.translation.y << ", "
              << t.transform.translation.z << ")");
          RCLCPP_INFO_STREAM(this->get_logger(),
              "rotation: (" << t.transform.rotation.x << ", "
              << t.transform.rotation.y << ", "
              << t.transform.rotation.z << ", "
              << t.transform.rotation.w << ")");
        } catch (const tf2::TransformException & ex) {
          RCLCPP_INFO(this->get_logger(), "Could not transform %s to %s: %s",
              toFrameRel.c_str(), fromFrameRel.c_str(), ex.what());
          return;
        }

        geometry_msgs::msg::Twist msg;

        static const double scaleRotationRate = 1.0;
        msg.angular.z = scaleRotationRate * atan2(
          t.transform.translation.y,
          t.transform.translation.x);

        static const double scaleForwardSpeed = 0.5;
        msg.linear.x = scaleForwardSpeed * sqrt(
          pow(t.transform.translation.x, 2) +
          pow(t.transform.translation.y, 2));

        publisher_->publish(msg);
      } else {
        RCLCPP_INFO(this->get_logger(), "Successfully spawned");
        turtle_spawned_ = true;
      }
    } else {
      // Check if the service is ready
      if (spawner_->service_is_ready()) {
        // Initialize request with turtle name and coordinates
        // Note that x, y and theta are defined as floats in turtlesim/srv/Spawn
        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
        request->x = 4.0;
        request->y = 2.0;
        request->theta = 0.0;
        request->name = "turtle2";

        // Call request
        using ServiceResponseFuture =
          rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture;
        auto response_received_callback = [this](ServiceResponseFuture future) {
            auto result = future.get();
            if (strcmp(result->name.c_str(), "turtle2") == 0) {
              turtle_spawning_service_ready_ = true;
            } else {
              RCLCPP_ERROR(this->get_logger(), "Service callback result mismatch");
            }
          };
        auto result = spawner_->async_send_request(request, response_received_callback);
      } else {
        RCLCPP_INFO(this->get_logger(), "Service is not ready");
      }
    }
  }

  // Boolean values to store the information
  // if the service for spawning turtle is available
  bool turtle_spawning_service_ready_;
  // if the turtle was successfully spawned
  bool turtle_spawned_;
  rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr spawner_{nullptr};
  rclcpp::TimerBase::SharedPtr timer_{nullptr};
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_{nullptr};
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_{nullptr};
  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  std::string target_frame_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FrameListener>());
  rclcpp::shutdown();
  return 0;
}
