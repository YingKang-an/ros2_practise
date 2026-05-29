#include <iostream>
#include <filesystem>
#include <thread>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include "behaviortree_cpp/loggers/groot2_publisher.h"

using namespace std::chrono_literals;

struct Position2D
{
  double x;
  double y;
};

// 为了让 "XML加载器" 能把字符串变成 Position2D 这种自定义类型
// 我们必须自己写一个转换函数,告诉BehaviorTree怎么把字符串→转成Position2D
namespace BT
{

template <> inline Position2D convertFromString(StringView str) {
  auto parts = splitString(str, ';');
  if (parts.size() != 2) {
    throw RuntimeError("invalid input!");
  }
  Position2D out;
  out.x = convertFromString<double>(parts[0]);
  out.y = convertFromString<double>(parts[1]);
  return out;
}

} // namespace BT

class SetTemplateBlackboard : public BT::SyncActionNode {
public:
 SetTemplateBlackboard(const std::string& name, const BT::NodeConfig& cfg)
  : SyncActionNode(name, cfg)
  {
  }

  static BT::PortsList providedPorts() {
    return {
      BT::OutputPort<Position2D>("points"),
      BT::OutputPort<int>("hp")
    };
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(2s);
    Position2D pos {10.5, 30.23};
    int hp = 99;
    setOutput("points", pos);
    setOutput("hp", hp);
    std::cout << "写入：x=" << pos.x << " y=" << pos.y << " hp=" << hp << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

class ReadPoints : public BT::SyncActionNode {
public:
  ReadPoints(const std::string& name, const BT::NodeConfig& cfg)
    : BT::SyncActionNode(name, cfg)
  {
  }

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<Position2D>("points")
    };
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(2s);
    auto res = getInput<Position2D>("points");
    if (!res) return BT::NodeStatus::FAILURE;
    auto value = res.value();
    std::cout << "读到 Position2D: x=" << value.x << " y=" << value.y << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

class ReadHp : public BT::SyncActionNode {
public:
  ReadHp(const std::string& name, const BT::NodeConfig& cfg)
    : BT::SyncActionNode(name, cfg)
  {
  }

  static BT::PortsList providedPorts() {
    return {
      BT::InputPort<int>("hp")
    };
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(2s);
    auto res = getInput<int>("hp");
    if (!res) return BT::NodeStatus::FAILURE;
    std::cout << "读到 int: " << res.value() << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

int main() {
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<SetTemplateBlackboard>("SetTemplateBlackboard");
  factory.registerNodeType<ReadPoints>("ReadPoints");
  factory.registerNodeType<ReadHp>("ReadHp");

  auto path = ament_index_cpp::get_package_share_directory(
      "behavior_tree_cpp") + "/trees/template_blackboard.xml";
  auto tree = factory.createTreeFromFile(path);
  BT::Groot2Publisher publish(tree, 5555);
  tree.tickWhileRunning();
  std::this_thread::sleep_for(2s);
  
  return 0;
}
