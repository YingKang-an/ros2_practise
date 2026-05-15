#include <memory>
#include "rclcpp/rclcpp.hpp"
// 几何消息包/消息类型定义/TransformStamped 消息格式
#include "geometry_msgs/msg/transform_stamped.hpp"
// 坐标变换库/坐标变换库/四元数
#include "tf2/LinearMath/Quaternion.hpp"
// ROS2/静态坐标变换广播器工具
#include "tf2_ros/static_transform_broadcaster.hpp"

class StaticFramePublisher : public rclcpp::Node {
public:
  explicit StaticFramePublisher(char* transformation[]) : Node(
      "static_tf2_broadcaster") {
    tf_static_broadcaster_ =
        std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
    // Publish static transforms once at startup
    this->make_transforms(transformation);
  }

private:
  void make_transforms(char* transformation[]) {
    geometry_msgs::msg::TransformStamped t;

    t.header.stamp = this->get_clock()->now(); // 当前时间来加盖章
    t.header.frame_id = "world";               // 父框名称
    t.child_frame_id = transformation[1];      // 子帧名称

    t.transform.translation.x = atof(transformation[2]);
    t.transform.translation.y = atof(transformation[3]);
    t.transform.translation.z = atof(transformation[4]);
    tf2::Quaternion q;
    q.setRPY(
      atof(transformation[5]),
      atof(transformation[6]),
      atof(transformation[7]));
    // q = xi + yj + zk + w
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();

    tf_static_broadcaster_->sendTransform(t);
  }

private:
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_static_broadcaster_;
};

int main(int argc, char * argv[])
{
  auto logger = rclcpp::get_logger("logger");

  // Obtain parameters from command line arguments
  if (argc != 8) {
    RCLCPP_INFO(
      logger, "Invalid number of parameters\nusage: "
      "$ ros2 run learning_tf2_cpp static_turtle_tf2_broadcaster "
      "child_frame_name x y z roll pitch yaw");
    return 1;
  }

  // As the parent frame of the transform is `world`, it is
  // necessary to check that the frame name passed is different
  if (strcmp(argv[1], "world") == 0) {
    RCLCPP_INFO(logger, "Your static turtle name cannot be 'world'");
    return 2;
  }

  // Pass parameters and initialize node
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<StaticFramePublisher>(argv));
  rclcpp::shutdown();
  return 0;
}
