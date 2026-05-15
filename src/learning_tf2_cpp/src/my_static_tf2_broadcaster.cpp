#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2_ros/static_transform_broadcaster.hpp>

using namespace std::chrono_literals;

class Tf2Broadcaster : public rclcpp::Node {
public:
  explicit Tf2Broadcaster() : Node("tf_broadcaster") {
    tf2_static_boastcarder_ = 
        std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

    geometry_msgs::msg::TransformStamped t;
    t.header.stamp = this->get_clock()->now();
    // 在 moving_frame 坐标系下的坐标，转换到 world 坐标系下
    t.header.frame_id = "world";
    t.child_frame_id = "static_frame";
    t.transform.translation.x = 1;
    t.transform.translation.y = 1;
    t.transform.translation.z = 1;
    t.transform.rotation.x = 0.0;
    t.transform.rotation.y = 0.0;
    t.transform.rotation.z = 0.0;
    t.transform.rotation.w = 1.0;
    tf2_static_boastcarder_->sendTransform(t);
  }

private:
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf2_static_boastcarder_;
  std::shared_ptr<rclcpp::TimerBase> timer_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Tf2Broadcaster>());
  rclcpp::shutdown();
  return 0;
}
