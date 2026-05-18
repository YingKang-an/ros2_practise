#include <iostream>
#include <memory>

// 行为树节点 基类
class BehaviorTreeNode {
public:
  // 销毁对象时，保证子类一起清理内存，别漏内存！
  virtual ~BehaviorTreeNode() = default;
  // 纯虚方法，必须子类实现
  virtual void tick() = 0; 
};

// Hello 节点(子类)
class HelloNode : public BehaviorTreeNode {
public:
  void tick() override {
    std::cout << "Hello 节点运行！内存地址: " << this << "\n";
  }
};

// 节点工厂类
class NodeFactory {
public:
  std::unique_ptr<HelloNode> createHelloNode() {
    // 核心实现：新建一个 HelloNode 对象
    return std::make_unique<HelloNode>(); /**< make_unique自动移动构造 */
  }
};

int main() {
  NodeFactory factory;
  // 工厂创建 3 个独立实例
  auto node1 = factory.createHelloNode();
  auto node2 = factory.createHelloNode();
  auto node3 = factory.createHelloNode();
  // 执行节点
  node1->tick();
  node2->tick();
  node3->tick();
  return 0;
}
