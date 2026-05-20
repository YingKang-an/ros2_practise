#include "behavior_tree_cpp/blackboard_nodes.h"

namespace BlackboardNodes
{

SaySomething::SaySomething(const std::string& name, const BT::NodeConfig& config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList SaySomething::providedPorts()
{
  return { BT::InputPort<std::string>("message") };
}

BT::NodeStatus SaySomething::tick()
{
  auto msg = getInput<std::string>("message");
  if (!msg)
  {
    throw BT::RuntimeError("missing required input [message]: ", msg.error());
  }
  std::cout << "Robot says: " << msg.value() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

ThinkWhatToSay::ThinkWhatToSay(const std::string& name, const BT::NodeConfig& config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList ThinkWhatToSay::providedPorts()
{
  return { BT::OutputPort<std::string>("text") };
}

BT::NodeStatus ThinkWhatToSay::tick()
{
  const std::string answer = "The answer is 42";
  setOutput("text", answer);
  std::cout << "ThinkWhatToSay wrote: " << answer << std::endl;
  return BT::NodeStatus::SUCCESS;
}

void registerBlackboardNodes(BT::BehaviorTreeFactory& factory)
{
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<ThinkWhatToSay>("ThinkWhatToSay");
}

}  // namespace BlackboardNodes
