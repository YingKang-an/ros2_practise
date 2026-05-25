#include <iostream>
#include <chrono>
#include <filesystem>
#include <thread>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/condition_node.h"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/loggers/groot2_publisher.h"

using namespace std::chrono_literals;

BT::NodeStatus checkBattery() {
  std::this_thread::sleep_for(2s);
  std::cout << "[ Battery: OK ]" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

class approachObject : public BT::SyncActionNode {
public:
  approachObject(const std::string& name) : BT::SyncActionNode(name, {})
  {
  }

  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(2s);
    std::cout << this->name() << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

class GripperInterface {
public:
  GripperInterface() : _open(true)
  {
  }
    
  BT::NodeStatus open() {
    _open = true;
    std::this_thread::sleep_for(2s);
    std::cout << "GripperInterface::open" << std::endl;
    return BT::NodeStatus::SUCCESS;
  }

  BT::NodeStatus close() {
    std::cout << "GripperInterface::close" << std::endl;
    _open = false;
    std::this_thread::sleep_for(2s);
    return BT::NodeStatus::SUCCESS;
  }

private:
  bool _open; // shared information
};


int main() {
  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<approachObject>("ApproachObject");

  factory.registerSimpleCondition("CheckBattery", [&](BT::TreeNode&) { return checkBattery(); });

  GripperInterface gripper;
  factory.registerSimpleAction("OpenGripper", [&](BT::TreeNode&){ return gripper.open(); } );
  factory.registerSimpleAction("CloseGripper", [&](BT::TreeNode&){ return gripper.close(); } );

  const std::string pkg_share = ament_index_cpp::get_package_share_directory("behavior_tree_cpp");
  const std::filesystem::path xml_file = std::filesystem::path(pkg_share) / "trees" / "my_IDE_tree.xml";
  std::cout << "Loading tree file: " << xml_file << std::endl;
  auto tree = factory.createTreeFromFile(xml_file.string());

  BT::Groot2Publisher publish(tree, 5555);

  tree.tickWhileRunning();
}