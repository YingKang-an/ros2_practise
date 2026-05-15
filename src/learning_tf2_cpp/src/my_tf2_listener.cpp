#include <memory>
#include <iomanip>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/exceptions.h"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"

using namespace std::chrono_literals;

class SimpleTfListener : public rclcpp::Node {
public:
  explicit SimpleTfListener() : Node("my_tf2_listener") {
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    timer_ = this->create_wall_timer(500ms, std::bind(&SimpleTfListener::on_timer, this));
  }

private:
  void on_timer()
  {
    const std::string target = "world";
    const std::string source = "moving_frame";
    try {
      geometry_msgs::msg::TransformStamped t =
        tf_buffer_->lookupTransform(target, source, tf2::TimePointZero);

      RCLCPP_INFO_STREAM(this->get_logger(), "frame_id: " << t.header.frame_id);
      RCLCPP_INFO_STREAM(this->get_logger(), "stamp: " << t.header.stamp.sec << "." 
        << std::setw(9) << std::setfill('0') << t.header.stamp.nanosec);
      RCLCPP_INFO_STREAM(this->get_logger(), "child_frame_id: " << t.child_frame_id);
      RCLCPP_INFO_STREAM(this->get_logger(), "translation: (" << t.transform.translation.x << ", "
                        << t.transform.translation.y << ", " << t.transform.translation.z << ")");
      RCLCPP_INFO_STREAM(this->get_logger(), "rotation: (" << t.transform.rotation.x << ", "
                        << t.transform.rotation.y << ", " << t.transform.rotation.z << ", "
                        << t.transform.rotation.w << ")");
    } catch (const tf2::TransformException & ex) {
      RCLCPP_INFO(this->get_logger(), "Could not transform %s to %s: %s",
                  target.c_str(), source.c_str(), ex.what());
    }
  }

  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  rclcpp::TimerBase::SharedPtr timer_{nullptr};
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SimpleTfListener>());
  rclcpp::shutdown();
  return 0;
}
