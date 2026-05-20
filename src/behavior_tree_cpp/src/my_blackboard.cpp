#include "behavior_tree_cpp/blackboard_nodes.h"
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include <filesystem>
#include <iostream>

int main()
{
  BT::BehaviorTreeFactory factory;
  BlackboardNodes::registerBlackboardNodes(factory);

  const std::string pkg_share = 
      ament_index_cpp::get_package_share_directory("behavior_tree_cpp");
  const std::filesystem::path xml_file = 
      std::filesystem::path(pkg_share)/"trees"/"my_blackboard_tree.xml";

  std::cout << "Loading tree file: " << xml_file << std::endl;
  auto tree = factory.createTreeFromFile(xml_file.string());

  std::cout << "Starting Blackboard Behavior Tree..." << std::endl;
  tree.tickWhileRunning();
  std::cout << "Behavior Tree finished." << std::endl;

  return 0;
}
