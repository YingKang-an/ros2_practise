#!/usr/bin/env python3

import rclpy
# from rclpy import Node
from rclpy.lifecycle import LifecycleNode
from rclpy.lifecycle.node import LifecycleState, TransitionCallbackReturn
from example_interfaces.msg import Int64


class NumberLifecyclePublisherNode(LifecycleNode):
  def __init__(self):
    # 未配置 阶段：初始化,仅定义基础变量
    super().__init__("number_publisher")
    self.get_logger().info("IN constructor")
    self.number_ = 1
    self.publish_frequency_ = 1.0
    self.number_publisher_ = None
    self.number_timer_ = None

  # --------------------------------------------------------------------

  def on_configure(self, previous_state:LifecycleState) -> TransitionCallbackReturn:
    # 未配置 -> 非活跃 阶段：创建ros2通讯,参数,硬件连接,不启动定时器
    self.get_logger().info("Lifecycle node configured")
    self.number_publisher_ = self.create_lifecycle_publisher(Int64, "number", 10)
    self.number_timer_ = self.create_timer(1.0 / self.publish_frequency_,
        self.publish_number)
    # 如果不取消,定时器会调用,但不会发布话题
    self.number_timer_.cancel()
    return TransitionCallbackReturn.ERROR ###############

  def on_cleanup(self, previous_state:LifecycleState) -> TransitionCallbackReturn:
    # 非活跃 -> 未配置 阶段：销毁ros2通讯,参数,硬件连接
    self.get_logger().info("IN on_cleanup")
    self.destroy_lifecycle_publisher(self.number_publisher_)
    self.destroy_timer(self.number_timer_)
    raise Exception() ############
    return TransitionCallbackReturn.SUCCESS

  # --------------------------------------------------------------------

  def on_activate(self, previous_state:LifecycleState):
    # 非活跃 -> 活跃 阶段：激活/使能 硬件
    self.get_logger().info("IN on_activate")
    self.number_timer_.reset()
    return super().on_activate(previous_state)

  def on_deactivate(self, previous_state:LifecycleState):
    # 活跃 -> 非活跃 阶段：不 激活/使能 硬件
    self.get_logger().info("IN on_deactivate")
    self.number_timer_.cancel()
    return super().on_deactivate(previous_state)

  # --------------------------------------------------------------------

  def on_shutdown(self, previous_state:LifecycleState):
    # 任何状态 -> 关闭状态：销毁所有资源,关闭节点
    self.get_logger().info("IN on_shutdown")
    self.destroy_lifecycle_publisher(self.number_publisher_)
    self.destroy_timer(self.number_timer_)
    return TransitionCallbackReturn.SUCCESS
  
  # --------------------------------------------------------------------

  def on_error(self, previous_state:LifecycleState):
    self.get_logger().error("IN on_error")
    self.destroy_lifecycle_publisher(self.number_publisher_)
    self.destroy_timer(self.number_timer_)
    # 做一些检查,如果可以修复,return SUCCESS,否则 return FAILURE
    return TransitionCallbackReturn.SUCCESS

  # --------------------------------------------------------------------
  def publish_number(self):
    msg = Int64()
    msg.data = self.number_
    self.number_publisher_.publish(msg)
    self.number_ += 1


def main(args=None):
  rclpy.init(args=args)
  node = NumberLifecyclePublisherNode()
  rclpy.spin(node)
  rclpy.shutdown()