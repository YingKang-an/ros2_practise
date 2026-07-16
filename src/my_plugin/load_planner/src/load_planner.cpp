/*****
 * 创建一个类加载器
 * 加载插件
 * 调用插件的函数
 */

#include <rclcpp/rclcpp.hpp>
#include <pluginlib/class_loader.hpp>
#include <navigation_planner/base_planner.hpp>

class LoadPlanner: public rclcpp::Node
{
public:
  LoadPlanner() : Node("load_planner") {
    this->declare_parameter("planner_plugin", "planner_sample_1::Planner1");
    RCLCPP_INFO(this->get_logger(), "创建了一个加载器的节点");
    // 从插件类名中提取包名 (格式: package::ClassName)
    std::string plugin_name = this->get_parameter("planner_plugin").as_string();
    std::string package_name = plugin_name.substr(0, plugin_name.find("::"));
    // 创建一个类加载器
    pluginlib::ClassLoader<nav_planner::BasePlanner> loader(package_name, "nav_planner::BasePlanner");
    // 加载插件
    auto planner = loader.createSharedInstance(plugin_name);
    // 调用插件的函数
    planner->init(10.0);
    planner->path();
  }
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<LoadPlanner>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}