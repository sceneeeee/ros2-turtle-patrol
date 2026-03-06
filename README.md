# ros2-turtle-patrol

这是一个基于 **ROS 2 + turtlesim + C++** 的练习项目，用于学习自定义服务接口、服务端/客户端通信，以及通过话题控制小海龟移动。

项目包含一个客户端和一个服务端：

- 客户端周期性发送随机目标坐标
- 服务端接收目标坐标并更新目标点
- 控制节点订阅小海龟当前位置，并发布速度指令，让海龟移动到目标位置

---

## 项目结构

```bash
turtle_ws/
├── src/
│   ├── chapt4_turtle_interfaces/
│   │   └── srv/
│   │       └── Patrol.srv
│   └── demo_cpp_service/
│       └── src/
│           ├── patrol_client.cpp
│           └── turtle_control.cpp
```


1. chapt4_turtle_interfaces

自定义接口包，定义了 Patrol.srv 服务。

2. demo_cpp_service

功能包，包含两个 C++ 节点：
patrol_client.cpp：服务客户端，定时发送随机巡逻目标点
turtle_control.cpp：服务端和控制节点，接收目标点并控制海龟移动

功能说明

patrol_client
客户端节点会：
创建 patrol 服务客户端
每隔 10 秒检查服务是否在线
随机生成目标坐标 target_x 和 target_y
向服务端发送请求
根据服务端响应输出成功或失败日志

turtle_control
服务端节点会：
创建 patrol 服务
接收客户端发送的目标坐标
判断目标坐标是否合法
若合法，则更新目标位置
订阅 /turtle1/pose 获取当前海龟位置
发布 /turtle1/cmd_vel 控制海龟向目标点移动

实现思路

该项目的核心思路如下：
客户端通过 ROS 2 service 向服务端发送目标点
服务端保存目标点坐标
控制节点实时读取海龟当前位置
计算当前点到目标点的距离和角度误差
当角度偏差较大时，先调整朝向
当朝向基本正确时，向前移动
重复以上过程，直到接近目标点
控制策略采用了简单的比例控制：
线速度与目标距离成正比
当角度误差较大时优先旋转
对线速度设置最大值限制，避免移动过快

依赖环境

ROS 2
rclcpp
geometry_msgs
turtlesim
colcon

编译方法

在 workspace 根目录下执行：

cd turtle_ws
colcon build
source install/setup.bash
运行方法
1. 启动 turtlesim
ros2 run turtlesim turtlesim_node
2. 启动服务端控制节点
source install/setup.bash
ros2 run demo_cpp_service turtle_control
3. 启动客户端节点
source install/setup.bash
ros2 run demo_cpp_service patrol_client

运行后，客户端会周期性发送随机目标点，小海龟会朝目标点移动。

学习内容

通过这个项目，我主要练习了以下内容：
ROS 2 workspace 和 package 的基本组织方式
自定义 .srv 接口
ROS 2 C++ 服务端与客户端通信
publisher / subscriber 的使用
turtlesim 速度控制
基于当前位置和目标位置的简单运动控制逻辑
当前项目特点
这个项目是学习 ROS 2 过程中的一个练习项目，重点在于：
理解 ROS 2 service 通信流程
学习如何把自定义接口包和功能包分开组织
使用 turtlesim 做一个可视化的控制示例
后续可以改进的方向
后续还可以继续完善，例如：
优化角度控制逻辑
增加目标点合法性判断
改进随机坐标生成范围
支持多个巡逻点
实现固定路线巡逻，而不是随机目标点
增加更完整的日志输出和状态反馈

说明

这是我在学习 ROS 2 过程中的实践项目，主要用于练习服务通信与小海龟控制。
