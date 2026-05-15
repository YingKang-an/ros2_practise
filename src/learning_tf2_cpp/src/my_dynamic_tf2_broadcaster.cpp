#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2_ros/transform_broadcaster.hpp>

using namespace std::chrono_literals;

class Tf2Broadcaster : public rclcpp::Node {
public:
  explicit Tf2Broadcaster() : Node("tf2_broadcaster") {
    tf2_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(this);
    timer_ = this->create_wall_timer(1s, [this](){ this->callback(); });
  }
private:
  void callback() {
    geometry_msgs::msg::TransformStamped t;
    t.header.stamp = this->get_clock()->now();
    // 在 moving_frame 坐标系下的坐标，转换到 world 坐标系下
    t.header.frame_id = "world";
    t.child_frame_id = "moving_frame";

    // move in a circle on the XY plane
    t.transform.translation.x = std::cos(angle_);
    t.transform.translation.y = std::sin(angle_);
    t.transform.translation.z = 0.0;

    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, angle_);
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();

    tf2_broadcaster_->sendTransform(t);

    angle_ += 0.02;
    if (angle_ > 2.0 * M_PI) { angle_ -= 2.0 * M_PI; }
  }
private:
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf2_broadcaster_{nullptr};
  std::shared_ptr<rclcpp::TimerBase> timer_{nullptr};
  double angle_{0.0};
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Tf2Broadcaster>());
  rclcpp::shutdown();
  return 0;
}