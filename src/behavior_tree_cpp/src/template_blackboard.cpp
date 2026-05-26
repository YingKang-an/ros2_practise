#include <iostream>
#include <filesystem>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp/bt_factory.h>

struct Position2D
{
  double x;
  double y;
};

// 为了让 XML 加载器能把字符串变成 Position2D 这种自定义类型
// 我们必须自己写一个转换函数,告诉BehaviorTree怎么把字符串→转成Position2D
namespace BT
{

template <> // 显式特化
inline Position2D convertFromString(StringView str)
{
  auto parts = splitString(str, ';');
  Position2D out;
  out.x = convertFromString<double>(parts[0]);
  out.y = convertFromString<double>(parts[1]);
  return out;
}

} // namespace BT

// ------------------------------
// 写入节点
// ------------------------------
class TemplateBlackboard : public BT::SyncActionNode
{
public:
    TemplateBlackboard(const std::string& name, const BT::NodeConfig& cfg)
        : SyncActionNode(name, cfg) {}

    static BT::PortsList providedPorts()
    {
        return {
            BT::OutputPort<Position2D>("point2f"),
            BT::OutputPort<int>("hp")
        };
    }

    BT::NodeStatus tick() override
    {
        Position2D pos {10.5, 20.7};
        int hp = 99;
        setOutput("point2f", pos);
        setOutput("hp", hp);
        std::cout << "写入：x=" << pos.x << " y=" << pos.y << " hp=" << hp << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

// ------------------------------
// 读取节点（适配你的XML！）
// ------------------------------
class ReadBlackboard : public BT::SyncActionNode
{
public:
    ReadBlackboard(const std::string& name, const BT::NodeConfig& cfg)
        : BT::SyncActionNode(name, cfg) {}

    static BT::PortsList providedPorts()
    {
        return { BT::InputPort<BT::Any>("target") };
    }

    BT::NodeStatus tick() override
    {
        auto res = getInput<BT::Any>("target");
        if (!res) return BT::NodeStatus::FAILURE;

        // 读取 int
        try {
            int value = res.value().cast<int>();
            std::cout << "读到 int: " << value << std::endl;
            return BT::NodeStatus::SUCCESS;
        } catch (...) {}

        // 读取 Position2D
        try {
            Position2D value = res.value().cast<Position2D>();
            std::cout << "读到 Position2D: x=" << value.x << " y=" << value.y << std::endl;
            return BT::NodeStatus::SUCCESS;
        } catch (...) {}

        return BT::NodeStatus::SUCCESS;
    }
};

// ------------------------------
// main
// ------------------------------
int main() {
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<TemplateBlackboard>("TemplateBlackboard");
  factory.registerNodeType<ReadBlackboard>("ReadBlackboard");

  auto path = ament_index_cpp::get_package_share_directory("behavior_tree_cpp") + "/trees/template_blackboard.xml";
  auto tree = factory.createTreeFromFile(path);
  tree.tickWhileRunning();

  return 0;
}
