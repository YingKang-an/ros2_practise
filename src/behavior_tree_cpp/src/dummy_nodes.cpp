#include "behavior_tree_cpp/dummy_nodes.h"
#include <iostream>

namespace DummyNodes
{

ApproachObject::ApproachObject(const std::string& name)
: BT::SyncActionNode(name, {})
{}

BT::NodeStatus ApproachObject::tick()
{
  std::cout << "ApproachObject: " << name() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

CheckBattery::CheckBattery(const std::string& name)
: BT::ConditionNode(name, {})
{}

BT::NodeStatus CheckBattery::tick()
{
  std::cout << "[ Battery: OK ]" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

OpenGripper::OpenGripper(const std::string& name)
: BT::SyncActionNode(name, {}), _open(false)
{}

BT::NodeStatus OpenGripper::tick()
{
  _open = true;
  std::cout << "OpenGripper: " << name() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

CloseGripper::CloseGripper(const std::string& name)
: BT::SyncActionNode(name, {}), _open(false)
{}

BT::NodeStatus CloseGripper::tick()
{
  _open = false;
  std::cout << "CloseGripper: " << name() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

void registerDummyNodes(BT::BehaviorTreeFactory& factory)
{
  factory.registerNodeType<ApproachObject>("ApproachObject");
  factory.registerNodeType<CheckBattery>("CheckBattery");
  factory.registerNodeType<OpenGripper>("OpenGripper");
  factory.registerNodeType<CloseGripper>("CloseGripper");
}

}  // namespace DummyNodes
