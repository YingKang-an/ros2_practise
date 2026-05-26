// test.cpp
#include "ament_index_cpp/get_package_share_directory.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/json_export.h"
#include "behaviortree_cpp/loggers/groot2_publisher.h"
#include "behaviortree_cpp/xml_parsing.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

#define BT_GENERATE_MODEL_ONLY 0

// 自定义坐标类型
struct Position2D
{
  double x;
  double y;
};

namespace BT
{
template <>
inline Position2D convertFromString(StringView str)
{
  auto parts = splitString(str, ';');
  if (parts.size() != 2)
  {
    throw RuntimeError("Position2D must be in format 'x;y'");
  }

  Position2D output;
  output.x = convertFromString<double>(parts[0]);
  output.y = convertFromString<double>(parts[1]);
  return output;
}

} // namespace BT

// JSON序列化，Groot可查看
BT_JSON_CONVERTER(Position2D, pos)
{
  add_field("x", &pos.x);
  add_field("y", &pos.y);
}

// 自定义动作节点
class CalculateGoal : public BT::SyncActionNode
{
public:
  CalculateGoal(const std::string& name, const BT::NodeConfig& cfg)
  : BT::SyncActionNode(name, cfg) {}

  static BT::PortsList providedPorts() {
    return { BT::OutputPort<Position2D>("goal") };
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Position2D mygoal = {1.1, 2.3};
    setOutput("goal", mygoal);
    return BT::NodeStatus::SUCCESS;
  }
};

class PrintTarget : public BT::SyncActionNode
{
public:
  PrintTarget(const std::string& name, const BT::NodeConfig& cfg)
  : BT::SyncActionNode(name, cfg) {}

  static BT::PortsList providedPorts() {
    const char* description = "Simply print the goal on console...";
    return { BT::InputPort<Position2D>("target", description) };
  }

  BT::NodeStatus tick() override
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto res = getInput<Position2D>("target");
    if (!res) {
      throw BT::RuntimeError("error reading port [target]: ", res.error());
    }
    Position2D target = res.value();
    printf("Target positions: [ %.1f, %.1f ]\n", target.x, target.y);
    return BT::NodeStatus::SUCCESS;
  }
};

int main() {
  BT::BehaviorTreeFactory factory;

  // 注册自定义节点
  factory.registerNodeType<CalculateGoal>("CalculateGoal");
  factory.registerNodeType<PrintTarget>("PrintTarget");

  // 注册自定义类型
  BT::RegisterJsonDefinition<Position2D>();

  // 只使用源码目录下的 trees 文件夹，不再写入 install 目录
  std::filesystem::path tree_dir = std::filesystem::current_path();
  while (true) {
    std::filesystem::path candidate = tree_dir / "src" / "behavior_tree_cpp" / "trees";
    if (std::filesystem::exists(candidate)) {
      tree_dir = candidate;
      break;
    }
    if (!tree_dir.has_parent_path()) {
      std::cerr << "Error: cannot find src/behavior_tree_cpp/trees from current path. "
                << "Please run from the workspace root or add the tree directory manually." << std::endl;
      return 1;
    }
    tree_dir = tree_dir.parent_path();
  }
  std::filesystem::create_directories(tree_dir);

  std::filesystem::path model_path = tree_dir / "nodes_model.xml";
  std::filesystem::path tree_path = tree_dir / "nodes_model.xml";

#if BT_GENERATE_MODEL_ONLY
  std::string model_xml = BT::writeTreeNodesModelXML(factory);
  std::ofstream fout(model_path);
  fout << model_xml;
  fout.close();

  std::cout << "Generated model file: " << model_path << std::endl;
  std::cout << "Use Groot to edit the real tree and save it as " << tree_path << "." << std::endl;
  return 0;
#endif

  if (!std::filesystem::exists(tree_path)) {
    std::cerr << "Error: tree file not found at " << tree_path << "\n";
    std::cerr << "Please create the behavior tree XML in this directory before running with BT_GENERATE_MODEL_ONLY=0." << std::endl;
    return 1;
  }

  factory.registerBehaviorTreeFromFile(tree_path.string());
  auto tree = factory.createTree("MainTree");

  // 连接Groot2实时调试
  BT::Groot2Publisher publisher(tree, 5555);

  while (true) {
    tree.tickWhileRunning();

    // 将黑板内容导出成 JSON 供观察
    // 如果没有 RegisterJsonDefinition<Position2D>()，Position2D 的字段可能无法正确序列化显示
    auto blackboard_json = BT::ExportBlackboardToJSON(*tree.rootBlackboard());
    std::cout << "Blackboard JSON:\n" << blackboard_json.dump(2) << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  return 0;
}
