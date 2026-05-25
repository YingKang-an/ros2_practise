#include <iostream>
#include <filesystem>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp/bt_factory.h>

using namespace BT;

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
    Position2D out;
    out.x = convertFromString<double>(parts[0]);
    out.y = convertFromString<double>(parts[1]);
    return out;
}
}

// ------------------------------
// 写入节点
// ------------------------------
class TemplateBlackboard : public SyncActionNode
{
public:
    TemplateBlackboard(const std::string& name, const NodeConfig& cfg)
        : SyncActionNode(name, cfg) {}

    static PortsList providedPorts()
    {
        return {
            OutputPort<Position2D>("point2f"),
            OutputPort<int>("hp")
        };
    }

    NodeStatus tick() override
    {
        Position2D pos {10.5, 20.7};
        int hp = 99;
        setOutput("point2f", pos);
        setOutput("hp", hp);
        std::cout << "写入：x=" << pos.x << " y=" << pos.y << " hp=" << hp << std::endl;
        return NodeStatus::SUCCESS;
    }
};

// ------------------------------
// 读取节点（适配你的XML！）
// ------------------------------
class ReadBlackboard : public SyncActionNode
{
public:
    ReadBlackboard(const std::string& name, const NodeConfig& cfg)
        : SyncActionNode(name, cfg) {}

    static PortsList providedPorts()
    {
        return { InputPort<BT::Any>("target") };
    }

    NodeStatus tick() override
    {
        auto res = getInput<BT::Any>("target");
        if (!res) return NodeStatus::FAILURE;

        // 读取 int
        try {
            int value = res.value().cast<int>();
            std::cout << "读到 int: " << value << std::endl;
            return NodeStatus::SUCCESS;
        } catch (...) {}

        // 读取 Position2D
        try {
            Position2D value = res.value().cast<Position2D>();
            std::cout << "读到 Position2D: x=" << value.x << " y=" << value.y << std::endl;
            return NodeStatus::SUCCESS;
        } catch (...) {}

        return NodeStatus::SUCCESS;
    }
};

// ------------------------------
// main
// ------------------------------
int main()
{
  BehaviorTreeFactory factory;
  factory.registerNodeType<TemplateBlackboard>("TemplateBlackboard");
  factory.registerNodeType<ReadBlackboard>("ReadBlackboard");

  auto path = ament_index_cpp::get_package_share_directory("behavior_tree_cpp") + "/trees/template_blackboard.xml";
  auto tree = factory.createTreeFromFile(path);
  tree.tickWhileRunning();

  return 0;
}
