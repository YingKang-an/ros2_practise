#include "behavior_tree_cpp/dummy_nodes.h"
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include <iostream>
#include <filesystem>

int main() {
  // 第一步：创建「行为树工厂」
  // 作用：登记所有节点 → 组装行为树
  BT::BehaviorTreeFactory factory;
  DummyNodes::registerDummyNodes(factory);

  const std::string pkg_share = ament_index_cpp::get_package_share_directory("behavior_tree_cpp");
  const std::filesystem::path xml_file = std::filesystem::path(pkg_share) / "trees" / "my_first_tree.xml";
/////////////////////////////////////////
  std::cout << "Loading tree file: " << xml_file << std::endl;
  auto tree = factory.createTreeFromFile(xml_file.string());
////////////////////////////////////////////
  std::cout << "Starting Behavior Tree..." << std::endl;
  tree.tickWhileRunning();
  std::cout << "Behavior Tree finished." << std::endl;

  return 0;
}
