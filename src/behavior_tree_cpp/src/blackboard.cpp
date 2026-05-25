#include <iostream>
#include <filesystem>
#include <thread>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/loggers/groot2_publisher.h>

using namespace std::chrono_literals;

class saySomething : public BT::SyncActionNode {
public:
  // 如果你的Node有端口,你必须使用这个构造函数签名
  saySomething(const std::string& name, 
      const BT::NodeConfiguration& config) : BT::SyncActionNode(name, config)
  {
  }

  // 当自定义TreeNode有输入/输出端口时,这些端口必须在静态方法中声明
  static BT::PortsList providedPorts() {
    // 此操作有一个名为"message"的输入端口
    return { BT::InputPort<std::string>("message") };
  }
  
  // 强烈建议在tick()里面调用getInput(),
  // 代码应该期望输入的实际值在运行时改变,因此应该定期更新
  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(2s);
    // 来自名为message这个端口的输入,可以用模板方法TreeNode::getInput<T>(key)来读取
    BT::Expected<std::string> msg = getInput<std::string>("message");
    // 检查expected是否有效。如果不是，抛出它的错误
    if (!msg) {
      throw BT::RuntimeError("缺少所需输入[message]", msg.error());
    }
    // 使用方法value()提取有效消息
    std::cout << "输出内容：" << msg.value() << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

class thinkWhatToSay : public BT::SyncActionNode {
public:
  thinkWhatToSay(const std::string& name, 
      const BT::NodeConfiguration& config) : BT::SyncActionNode(name, config)
  {
  }

  // 定义"输出端口"
  static BT::PortsList providedPorts() {
    return { BT::OutputPort<std::string>("text") };
  }

  // //此操作将值写入端口"text"
  BT::NodeStatus tick() override {
    std::this_thread::sleep_for(2s);
    // 输出可能会在每个tick()发生变化.在这里,我们保持简单
    std::string answer = "the answer is 42";
    // 通过text端口，把数据发送出去
    setOutput("text", answer);
    std::cout << "已经把答案写入黑板" << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};


int main() {
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<saySomething>("SaySomething");
  factory.registerNodeType<thinkWhatToSay>("ThinkWhatToSay");

  // 通过 ROS 包 share 路径加载 XML
  const std::string pkg_share = ament_index_cpp::get_package_share_directory("behavior_tree_cpp");
  const std::filesystem::path xml_file = std::filesystem::path(pkg_share) / "trees" / "blackboard.xml";
  std::cout << "Loading tree file: " << xml_file << std::endl;

  auto tree = factory.createTreeFromFile(xml_file.string());

  // 开启Groot2实时监控
  BT::Groot2Publisher publisher(tree, 5555);

  std::cout << "行为树启动,等待Groot连接..." << std::endl;

  // 持续循环运行行为树
  tree.tickWhileRunning();
  
  return 0;
}