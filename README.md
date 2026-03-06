# ros2-turtle-patrol

这是一个基于 **ROS 2 + turtlesim + C++** 的练习项目，用来学习：

- 自定义 `.srv` 服务接口
- ROS 2 C++ 服务端 / 客户端通信
- publisher / subscriber 的基本使用
- 通过 `turtlesim` 控制小海龟移动到目标点

项目由一个接口包和一个功能包组成：客户端会定时生成随机目标坐标并发送服务请求，服务端接收坐标后更新目标点，随后控制小海龟向目标位置移动。

---

## 项目结构

```bash
turtle_ws/
├── src/
│   ├── chapt4_turtle_interfaces/
│   │   ├── CMakeLists.txt
│   │   ├── package.xml
│   │   └── srv/
│   │       └── Patrol.srv
│   └── demo_cpp_service/
│       ├── CMakeLists.txt
│       ├── package.xml
│       └── src/
│           ├── patrol_client.cpp
│           └── turtle_control.cpp
````

---

## 项目组成

### `chapt4_turtle_interfaces`

这是接口包，用来定义自定义服务 `Patrol.srv`。

客户端通过这个服务向服务端发送目标点坐标，服务端处理后返回结果。

### `demo_cpp_service`

这是功能包，包含两个 C++ 节点：

* `patrol_client.cpp`：服务客户端，定时发送随机目标点
* `turtle_control.cpp`：服务端和控制节点，接收目标点并控制海龟移动

---

## 功能说明

### 客户端 `patrol_client`

客户端节点的主要功能如下：

* 创建 `patrol` 服务客户端
* 每隔 10 秒检查服务是否可用
* 构造 `Patrol::Request`
* 随机生成 `target_x` 和 `target_y`
* 向服务端发送异步请求
* 根据服务端返回结果输出成功或失败日志

### 服务端 `turtle_control`

服务端节点的主要功能如下：

* 创建 `patrol` 服务
* 接收客户端发送的目标点坐标
* 判断坐标是否合法
* 如果合法，则更新目标点
* 订阅 `/turtle1/pose` 获取当前海龟位置
* 发布 `/turtle1/cmd_vel` 控制海龟向目标点移动

---

## 运行逻辑

整个项目的运行流程如下：

1. 启动 `turtlesim_node`
2. 启动 `turtle_control` 节点
3. 服务端创建 `patrol` 服务，并持续监听海龟当前位姿
4. 启动 `patrol_client` 节点
5. 客户端每隔 10 秒生成一次随机目标点
6. 客户端向服务端发送请求
7. 服务端判断目标点是否合法
8. 合法则更新目标坐标
9. 控制节点根据当前位置和目标位置计算控制量
10. 小海龟朝新的目标点移动

---

## 控制思路

服务端采用了一个简单的目标点控制方法：

1. 通过订阅 `/turtle1/pose` 获取当前坐标和朝向
2. 计算当前位置与目标点之间的距离
3. 计算目标方向与当前朝向之间的角度误差
4. 当角度偏差较大时优先旋转
5. 当角度偏差较小时向前移动
6. 对线速度设置最大值限制，避免移动过快

这个控制方法适合用来练习 ROS 2 中的消息通信和基础运动控制逻辑。

---

## 依赖环境

运行该项目通常需要以下环境或依赖：

* ROS 2
* `rclcpp`
* `geometry_msgs`
* `turtlesim`
* `colcon`

---

## 编译方法

在 workspace 根目录下执行：

```bash
cd turtle_ws
colcon build
source install/setup.bash
```

---

## 运行方法

### 1. 启动 turtlesim

```bash
ros2 run turtlesim turtlesim_node
```

### 2. 启动服务端控制节点

```bash
cd turtle_ws
source install/setup.bash
ros2 run demo_cpp_service turtle_control
```

### 3. 启动客户端节点

```bash
cd turtle_ws
source install/setup.bash
ros2 run demo_cpp_service patrol_client
```

运行后，客户端会周期性发送随机目标点，小海龟会朝目标点移动。

---

## 学习内容

通过这个项目，我主要练习了以下内容：

* ROS 2 workspace 和 package 的基本组织方式
* 自定义 `.srv` 接口的创建与使用
* ROS 2 C++ 服务端与客户端通信
* publisher / subscriber 的使用方法
* turtlesim 的速度控制
* 基于当前位置和目标位置的简单运动控制逻辑

---

## 当前项目特点

这是一个学习 ROS 2 过程中的练习项目，重点在于：

* 理解 ROS 2 service 通信流程
* 学习如何将接口包与功能包分开组织
* 通过 turtlesim 做一个可视化的控制示例
* 将服务通信与小海龟运动控制结合起来

---

## 可以继续改进的方向

这个项目后续还可以继续扩展，例如：

* 优化角度控制逻辑
* 对角度误差进行归一化处理
* 改进随机坐标生成范围
* 支持多个目标点
* 实现固定路线巡逻
* 增加更完整的状态反馈
* 优化日志输出内容

---

## 项目说明

这是我在学习 ROS 2 过程中的实践项目，主要用于练习自定义服务通信与 `turtlesim` 小海龟控制。

```
```
