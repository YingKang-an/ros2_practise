#ifndef BEHAVIOR_TREE_CPP__DUMMY_NODES_H
#define BEHAVIOR_TREE_CPP__DUMMY_NODES_H

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <iostream>
#include <string>

namespace DummyNodes
{

class ApproachObject : public BT::SyncActionNode
{
public:
  explicit ApproachObject(const std::string& name);
  BT::NodeStatus tick() override;
};

class CheckBattery : public BT::ConditionNode
{
public:
  explicit CheckBattery(const std::string& name);
  BT::NodeStatus tick() override;
};

class OpenGripper : public BT::SyncActionNode
{
public:
  explicit OpenGripper(const std::string& name);
  BT::NodeStatus tick() override;

private:
  bool _open;
};

class CloseGripper : public BT::SyncActionNode
{
public:
  explicit CloseGripper(const std::string& name);
  BT::NodeStatus tick() override;

private:
  bool _open;
};

void registerDummyNodes(BT::BehaviorTreeFactory& factory);

}  // namespace DummyNodes

#endif  // BEHAVIOR_TREE_CPP__DUMMY_NODES_H
