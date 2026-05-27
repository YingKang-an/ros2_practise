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

class IsDoorOpen : public BT::ConditionNode {
public:
  IsDoorOpen(const std::string& name, const BT::NodeConfig& cfg)
  : BT::ConditionNode(name, cfg)
  {}

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return BT::NodeStatus::FAILURE;
  }
};

class HaveKey : public BT::ConditionNode {
public:
  HaveKey(const std::string& name, const BT::NodeConfig& cfg)
  : BT::ConditionNode(name, cfg)
  {}

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return BT::NodeStatus::SUCCESS;
  }
};

class OpenDoor : public BT::SyncActionNode {
public:
  OpenDoor(const std::string& name, const BT::NodeConfig& cfg)
  : BT::SyncActionNode(name, cfg)
  {}

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return BT::NodeStatus::FAILURE;
  }
};

class UnLockDoor : public BT::SyncActionNode {
public:
  UnLockDoor(const std::string& name, const BT::NodeConfig& cfg)
  : BT::SyncActionNode(name, cfg)
  {}

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return BT::NodeStatus::SUCCESS;
  }
};

class SmashDoor : public BT::SyncActionNode {
public:
  SmashDoor(const std::string& name, const BT::NodeConfig& cfg)
  : BT::SyncActionNode(name, cfg)
  {}

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return BT::NodeStatus::SUCCESS;
  }
};

class EnterRoom : public BT::SyncActionNode {
public:
  EnterRoom(const std::string& name, const BT::NodeConfig& cfg)
  : BT::SyncActionNode(name, cfg)
  {}

  static BT::PortsList providedPorts() {
    return {};
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return BT::NodeStatus::SUCCESS;
  }
};

int main() {
  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<IsDoorOpen>("IsDoorOpen");
  factory.registerNodeType<HaveKey>("HaveKey");
  factory.registerNodeType<OpenDoor>("OpenDoor");
  factory.registerNodeType<UnLockDoor>("UnLockDoor");
  factory.registerNodeType<SmashDoor>("SmashDoor");
  factory.registerNodeType<EnterRoom>("EntrerRoom");

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

  std::filesystem::path tree_path = tree_dir / "basic_nodes.xml";

  if (!std::filesystem::exists(tree_path)) {
    std::cerr << "Error: tree file not found at " << tree_path << "\n";
    return 1;
  }

  factory.registerBehaviorTreeFromFile(tree_path.string());
  auto tree = factory.createTree("main");

  // 连接Groot2实时调试
  BT::Groot2Publisher publisher(tree, 5555);

  while (true) {
    // tree.haltTree();
    tree.tickWhileRunning();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
