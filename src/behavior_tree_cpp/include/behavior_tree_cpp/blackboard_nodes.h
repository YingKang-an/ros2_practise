#ifndef BEHAVIOR_TREE_CPP__BLACKBOARD_NODES_H
#define BEHAVIOR_TREE_CPP__BLACKBOARD_NODES_H

#include <behaviortree_cpp/bt_factory.h>
#include <iostream>
#include <string>

namespace BlackboardNodes
{

class SaySomething : public BT::SyncActionNode
{
public:
  SaySomething(const std::string& name, const BT::NodeConfig& config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

class ThinkWhatToSay : public BT::SyncActionNode
{
public:
  ThinkWhatToSay(const std::string& name, const BT::NodeConfig& config);

  static BT::PortsList providedPorts();

  BT::NodeStatus tick() override;
};

void registerBlackboardNodes(BT::BehaviorTreeFactory& factory);

}  // namespace BlackboardNodes

#endif  // BEHAVIOR_TREE_CPP__BLACKBOARD_NODES_H
