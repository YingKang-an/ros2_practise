from launch import LaunchDescription
from launch_ros.actions import Node
# 封装终端指令相关类----------------
# from launch.actions import ExecuteProcess
# from launch.substitutions import FindExecutable
# 参数声明与获取------------------
# from launch.actions import DeclareLaunchArgument
# from launch.substitutions import LaunchConfiguration
# 文件包含相关--------------------
# from launch.actions import IncludeLaunchDescription
# from launch.launch_description_sources import PythonLaunchDescriptionSource
# 分组相关------------------------
# from launch_ros.actions import PushRosNamespace
# from launch.actions import GroupAction
# 事件相关------------------------
# from launch.event_handlers import OnProcessStart, OnProcessExit
# from launch.actions import ExecuteProcess, RegisterEventHandler, LogInfo
# 获取功能包下share目录路径---------
from ament_index_python.packages import get_package_share_directory
import os

"""
  演示Node的使用
  参数说明
    :param: executable the name of the executable to find if a package
        is provided or otherwise a path to the executable to run.
    :param: package the package in which the node executable can be found
    :param: name the name of the node
    :param: namespace the ROS namespace for this Node
    :param: exec_name the label used to represent the process.
        Defaults to the basename of node executable.
    :param: parameters list of names of yaml files with parameter rules,
        or dictionaries of parameters.
    :param: remappings ordered list of 'to' and 'from' string pairs to be
        passed to the node as ROS remapping rules
    :param: ros_arguments list of ROS arguments for the node
    :param: arguments list of extra arguments for the node
"""

def generate_launch_description():
  turtle1=Node(
    package="turtlesim",
    executable="turtlesim_node",
    exec_name="my_lable",
    ros_arguments=[
      "--remap","__ns:=/t2"
    ],
    respawn=True # 自动重启
  )
  turtle2=Node(
    package="turtlesim",
    executable="turtlesim_node",
    name="TURTLE",
    # 参数 方式1 直接设置参数
    # parameters=[{"background_r": 255,"background_g": 0,"background_b": 0}]
    # 参数 方式2 读取yaml文件
    parameters=[os.path.join(get_package_share_directory("launch_py"),"config","turtle.yaml")]
  )
  return LaunchDescription([turtle1,turtle2])