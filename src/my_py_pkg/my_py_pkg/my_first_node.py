#!/usr/bin/env python3
import rclpy
from rclpy.node import Node

class MyNode(Node):
  def __init__(self):
    super().__init__("py_test")
    self.declare_parameter("count", 0)
    self.declare_parameter("period", 1)
    self.counter_ = self.get_parameter("count").value
    self.period_ = self.get_parameter("period").value
    self.get_logger().info("hello python node!")
    self.create_timer(self.period_, self.time_callback)
    
  def time_callback(self):
    self.get_logger().info("Hello " + str(self.counter_))
    self.counter_ += 1

def main(args = None):
  rclpy.init(args = args)
  node = MyNode()
  rclpy.spin(node)
  rclpy.shutdown()

if __name__ == "__main__":
  main()