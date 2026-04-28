#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from example_interfaces.msg import String

class SmartPhoneNode(Node):
  def __init__(self):
    super().__init__("smart_phone")
    self.subscriber_ = self.create_subscription(
      String, "robot_news", self.callback_robot_news, 10)

  def callback_robot_news(self, msg: String):
    self.get_logger().info(msg.data)

def main(args = None):
  rclpy.init(args = args)
  node = SmartPhoneNode()
  rclpy.spin(node)
  rclpy.shutdown()

if __name__ == "__main__":
  main()

# import rclpy
# from rclpy.node import Node
# from example_interfaces.msg import String

# class MyNode(Node):
#   def __init__(self):
#     super().__init__("Node")
#     self.subscriber_ = self.create_subscription(self,String,"A",self.f)

#   def f(self, msg: String):
#     self.get_logger().info(msg.data)

# def main(args = None):
#   rclpy.init(args = args)
#   node = MyNode()
#   rclpy.spin(node)
#   rclpy.shutdown()

# if __name__ == "__main__":
#   main()