#include <functional>
#include <memory>
#include <sstream>
#include <string>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"
#include "turtlesim/msg/pose.hpp"

class FramePublisher : public rclcpp::Node {
public:
  explicit FramePublisher() : Node("tf2_broadcaster") {
    // 从 ROS2 参数服务器中声明并获取一个名为 turtlename 的字符串参数，默认值是 "turtle"
    turtlename_ = this->declare_parameter<std::string>(
        "turtlename", "turtle");
    // 初始化 TF 坐标广播器
    tf_broadcaster_ =
      std::make_unique<tf2_ros::TransformBroadcaster>(this);
    // 订阅 turtle{1}{2}/pose 主题
    // 并在每条消息上调用 handle_turtle_pose 回调函数
    std::ostringstream stream;
    stream << "/" << turtlename_.c_str() << "/pose";
    std::string topic_name = stream.str();

    // lambda : [捕获列表](参数) -> 返回值 { 函数体 };
    // 给这个 lambda 表达式起一个名字, 把它当作一个可调用对象传给
    auto handle_turtle_pose = [this](const std::shared_ptr<turtlesim::msg::Pose> msg) {
        geometry_msgs::msg::TransformStamped t;
      // 读取消息内容并将其分配给对应的 tf 变量
      t.header.stamp = this->get_clock()->now();
      t.header.frame_id = "world";
      t.child_frame_id = turtlename_.c_str();
      // 海龟只存在于二维，因此从消息中获取 x 和 y 坐标，并将 z 坐标设置为 0
      t.transform.translation.x = msg->x;
      t.transform.translation.y = msg->y; /**< 转换.平移 */
      t.transform.translation.z = 0.0;
      // 出于同样的原因，乌龟只能围绕一个轴旋转
      // 这就是为什么我们将 x 和 y 轴的旋转设置为 0，并从消息中获取 z 轴的旋转
      tf2::Quaternion q;
      q.setRPY(0, 0, msg->theta);
      t.transform.rotation.x = q.x();
      t.transform.rotation.y = q.y(); /**< 转换.旋转 */
      t.transform.rotation.z = q.z();
      t.transform.rotation.w = q.w();
      // 发送变换
      tf_broadcaster_->sendTransform(t);
    };

    subscription_ = this->create_subscription<turtlesim::msg::Pose>(
      topic_name, 10,
      handle_turtle_pose);
  }

private:
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  std::string turtlename_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FramePublisher>());
  rclcpp::shutdown();
  return 0;
}
