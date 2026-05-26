# Groot2 + BT.CPP 新手最佳实践

## 1. 上半部分：通用流程总结

### 1.1 你要理解的三件事

- **注册节点**：在 C++ 程序里调用 `factory.registerNodeType<...>(...)`，告诉 BT.CPP 每个自定义节点 ID 对应哪个类。
- **生成模型**：在程序里调用 `BT::writeTreeNodesModelXML(factory)`，生成 `nodes_model.xml`，让 Groot2 识别你的自定义节点端口。
- **画树结构**：在 Groot2 里导入模型后，拖入节点并连接它们，保存成行为树 XML。

### 1.2 这三件事的作用

- 注册节点让程序能够实例化你的自定义节点。
- 生成模型让 Groot2 知道你的节点有哪些端口，并把它们显示在左侧候选区。
- 画树结构让你定义节点之间的执行顺序和黑板数据绑定。

这三件事是不同的步骤，不能把它们混成一步。

### 1.3 最简通用流程

1. 在 C++ 中实现你的自定义节点。
2. 在程序中注册这些节点。
3. 在程序中调用 `BT::writeTreeNodesModelXML(factory)` 生成 `nodes_model.xml`。
4. 在 Groot2 中导入 `nodes_model.xml`。
5. 在 Groot2 里拖节点并连接它们，保存行为树 XML，例如 `trees/my_tree.xml`。
6. 程序加载 `trees/my_tree.xml` 并运行行为树。

### 1.4 这一步的结果

- `nodes_model.xml`：Groot2 可以识别你的自定义节点端口。
- `trees/my_tree.xml`：你在 Groot2 中设计的树结构。
- 程序加载后按这个树结构执行。

## 2. 下半部分：泛型黑板接口示例

以下通过 `Position2D` 自定义类型的例子，按步骤走一遍。

### 2.1 定义自定义类型

```cpp
struct Position2D {
  double x;
  double y;
};
```

### 2.2 定义字符串转换器

当行为树 XML 中使用 `Position2D` 类型时，BT.CPP 需要知道如何将字符串转换为这种类型。

```cpp
namespace BT {

template <>
inline Position2D convertFromString(StringView str) {
  auto parts = splitString(str, ';');
  if (parts.size() != 2) {
    throw RuntimeError("Position2D must be in format 'x;y'");
  }
  Position2D output;
  output.x = convertFromString<double>(parts[0]);
  output.y = convertFromString<double>(parts[1]);
  return output;
}

} // namespace BT
```

这段代码的意思是：
- 用 `splitString(str, ';')` 按分号拆分字符串；
- 检查是否得到两个部分；
- 把两个部分转成 `double` 并赋值给 `Position2D`。

如果你改成 `"(3,2)"` 的格式，就要改成先去掉括号，再按逗号拆分。

### 2.3 让 Groot2 可视化 `Position2D`

如果你希望 Groot2 能显示 `Position2D` 的黑板内容，需要添加 JSON 转换器：

```cpp
#include "behaviortree_cpp/json_export.h"

BT_JSON_CONVERTER(Position2D, pos) {
  add_field("x", &pos.x);
  add_field("y", &pos.y);
}
```

并在 `main()` 中注册：

```cpp
BT::RegisterJsonDefinition<Position2D>();
```

这样做的作用是：
- 让 `ExportBlackboardToJSON()` 能正确序列化 `Position2D`；
- 让 Groot2 的黑板视图可以显示 `x` 和 `y`；
- 让调试时看到真实的自定义数据。

### 2.4 定义两个自定义节点

```cpp
class CalculateGoal : public BT::SyncActionNode {
public:
  CalculateGoal(const std::string& name, const BT::NodeConfig& cfg)
    : BT::SyncActionNode(name, cfg) {}

  static BT::PortsList providedPorts() {
    return { BT::OutputPort<Position2D>("goal") };
  }

  BT::NodeStatus tick() override {
    Position2D mygoal{1.1, 2.3};
    setOutput("goal", mygoal);
    return BT::NodeStatus::SUCCESS;
  }
};

class PrintTarget : public BT::SyncActionNode {
public:
  PrintTarget(const std::string& name, const BT::NodeConfig& cfg)
    : BT::SyncActionNode(name, cfg) {}

  static BT::PortsList providedPorts() {
    return { BT::InputPort<Position2D>("target", "Print the goal to console") };
  }

  BT::NodeStatus tick() override {
    auto res = getInput<Position2D>("target");
    if (!res) {
      throw BT::RuntimeError("error reading port [target]:", res.error());
    }
    Position2D target = res.value();
    printf("Target positions: [ %.1f, %.1f ]\n", target.x, target.y);
    return BT::NodeStatus::SUCCESS;
  }
};
```

### 2.5 注册节点并生成模型文件

在 `main.cpp` 中写：

```cpp
BT::BehaviorTreeFactory factory;
factory.registerNodeType<CalculateGoal>("CalculateGoal");
factory.registerNodeType<PrintTarget>("PrintTarget");
BT::RegisterJsonDefinition<Position2D>();

std::string models_xml = BT::writeTreeNodesModelXML(factory);
std::ofstream("nodes_model.xml") << models_xml;
```

这一步会生成 `nodes_model.xml`，让 Groot2 识别这些节点。

### 2.6 在 Groot2 里画树

在 Groot2 中：
- 导入 `nodes_model.xml`；
- 左侧候选区出现你的自定义节点；
- 拖出节点并连接；
- 保存行为树 XML，例如 `trees/my_tree.xml`。

Groot2 会自动生成这个行为树 XML 文件。

### 2.7 程序加载树并运行

```cpp
factory.registerBehaviorTreeFromFile("trees/my_tree.xml");
auto tree = factory.createTree("MainTree");
BT::Groot2Publisher publisher(tree);
tree.tickWhileRunning();
```

如果你只想运行行为树，不需要实时调试，可以不使用 `BT::Groot2Publisher`。

## 3. 推荐文件结构

- `src/behavior_tree_cpp/src/my_nodes.cpp`：定义 `Position2D`、`convertFromString`、节点类。
- `src/behavior_tree_cpp/src/main.cpp`：注册节点、生成 `nodes_model.xml`、加载行为树、启动执行。
- `src/behavior_tree_cpp/trees/my_tree.xml`：由 Groot2 画出的行为树结构。
- `src/behavior_tree_cpp/nodes_model.xml`：由程序自动生成并导入 Groot2 的模型文件。

## 4. 重点提示

- `nodes_model.xml` 不是行为树，它只是自定义节点的描述文件；
- `my_tree.xml` 是行为树结构文件，程序最终执行它；
- `BT::RegisterJsonDefinition<Position2D>()` 不影响行为树执行逻辑，但会影响黑板数据可视化；
- 如果你改成 `"(3,2)"` 格式，解析逻辑要先去掉括号，再按逗号拆分；
- 如果你不使用 Groot2，仍然可以手写行为树 XML，但推荐用 Groot2 画树。

## 5. 你今天实验时要关注的点

1. 是否在程序里注册了自定义节点？
2. 是否生成了 `nodes_model.xml`？
3. 是否在 Groot2 里导入了 `nodes_model.xml`？
4. 是否在 Groot2 里拖节点画树？
5. 是否保存了 `trees/my_tree.xml`？
6. 是否程序正常加载并运行 `trees/my_tree.xml`？

---

> author YinKang'an 2026-5-27