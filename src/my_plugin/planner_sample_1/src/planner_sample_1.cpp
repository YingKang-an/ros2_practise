/****
 * 插件实现
 *  1.继承基类,并重写基类函数
 *  2.注册插件
 */

#include <rclcpp/rclcpp.hpp>
#include "navigation_planner/base_planner.hpp"
#include <pluginlib/class_list_macros.hpp>

namespace planner_sample_1
{

class Planner1 : public nav_planner::BasePlanner, public rclcpp::Node
{
public:
  Planner1():Node("sample_planner_1") {
    RCLCPP_INFO(this->get_logger(), "创建了一个简单的路径规划器");
  }
  // 初始化函数
  void init(double rate) {
    RCLCPP_INFO(this->get_logger(), "路径规划器插件1已经初始化,频率是%lfHZ", rate);
  }
  // 路进规划函数
  void path() {
    RCLCPP_INFO(this->get_logger(), "生成规划路径");
  }
};

} // namespace planner_sample_1

PLUGINLIB_EXPORT_CLASS(planner_sample_1::Planner1, nav_planner::BasePlanner)