# ros2-turtle-patrol

一个基于 **ROS 2 + turtlesim + C++** 的小海龟巡逻练习项目。

项目通过 **自定义 Service 接口 + 发布/订阅控制** 的方式，让客户端周期性发送随机目标点，服务端接收目标点后控制 `turtlesim` 中的小海龟移动到指定位置。

这个项目适合用来练习：

- ROS 2 C++ 节点开发
- 自定义 `.srv` 服务接口
- ROS 2 Service 客户端 / 服务端通信
- `publisher / subscriber` 的基本使用
- `turtlesim` 的运动控制
- 基于目标点的简单比例控制
- ROS 2 launch 文件编写与参数传递

---

## 项目功能

本项目实现了一个完整的目标点控制流程：

1. 客户端每隔一段时间生成一个随机目标点
2. 客户端调用 `patrol` 服务，把目标点发送给服务端
3. 服务端判断目标点是否合法
4. 若合法，则更新当前目标点
5. 服务端持续订阅小海龟当前位置
6. 根据当前位置和目标点计算控制量
7. 发布速度指令到 `/turtle1/cmd_vel`
8. 小海龟朝新的目标点移动

从功能上说，它更准确地是一个“**随机目标点追踪**”项目，而不是固定路径巡逻项目。

---

## 项目结构

```text
ros2-turtle-patrol/
├── .gitignore
├── README.md
└── src/
    ├── chapt4_turtle_interfaces/
    │   ├── CMakeLists.txt
    │   ├── package.xml
    │   └── srv/
    │       └── Patrol.srv
    └── demo_cpp_service/
        ├── CMakeLists.txt
        ├── package.xml
        ├── launch/
        │   └── demo.launch.py
        └── src/
            ├── patrol_client.cpp
            └── turtle_control.cpp
````

---

## 功能包说明

### 1. `chapt4_turtle_interfaces`

这是接口包，用于定义自定义服务：

```srv
float32 target_x
float32 target_y
---
int8 SUCESS = 1
int8 FAIL = 0
int8 result
```

说明：

* `target_x`、`target_y`：客户端发送的目标点坐标
* `result`：服务端返回的处理结果
* `SUCESS` / `FAIL`：结果状态常量

注意：源码中常量名写的是 `SUCESS`，这是代码中的实际拼写。

---

### 2. `demo_cpp_service`

这是功能包，包含两个 C++ 节点和一个 launch 文件：

* `patrol_client.cpp`：服务客户端，周期性发送随机目标点
* `turtle_control.cpp`：服务端 + 控制节点，接收目标点并驱动小海龟移动
* `launch/demo.launch.py`：一键启动整个示例系统

---

## 节点说明

### 1. 客户端节点 `patrol_client`

节点名：

```text
turtle_patrol_client
```

主要功能：

* 创建 `patrol` 服务客户端
* 使用 10 秒定时器周期性触发请求
* 在发送请求前等待服务可用
* 随机生成目标点 `target_x` 和 `target_y`
* 异步发送服务请求
* 根据服务端响应输出成功或失败日志

客户端的随机目标点生成逻辑是：

```cpp
request->target_x = static_cast<float>(rand() % 15);
request->target_y = static_cast<float>(rand() % 15);
```

这意味着客户端会生成 `0 ~ 14` 之间的整数坐标。

---

### 2. 服务端节点 `turtle_control`

节点名：

```text
turtle_control
```

主要功能：

* 创建 `patrol` 服务
* 接收客户端发送的目标点
* 判断目标点是否合法
* 若合法则更新目标点
* 订阅 `/turtle1/pose` 获取当前海龟位姿
* 发布 `/turtle1/cmd_vel` 控制海龟运动

---

## 话题与服务关系

### Service

* `/patrol`

### Topic

* 订阅：`/turtle1/pose`
* 发布：`/turtle1/cmd_vel`

数据流关系如下：

```text
patrol_client
    └── 调用服务 /patrol
            ↓
      turtle_control
            ├── 订阅 /turtle1/pose
            └── 发布 /turtle1/cmd_vel
                    ↓
               turtlesim 中的 turtle1
```

---

## Launch 功能

项目现在已经加入了 `demo.launch.py`，可以一次性启动整个示例系统。

这个 launch 文件会：

* 声明一个 launch 参数 `launch_arg_bg`
* 启动 `turtlesim_node`
* 将 `launch_arg_bg` 作为 `background_g` 参数传给 `turtlesim_node`
* 启动 `patrol_client`
* 启动 `turtle_control`

默认参数：

```text
launch_arg_bg = 150
```

也就是说，默认会将 `turtlesim` 背景的绿色通道设置为 `150`。

---

## 运行逻辑

整个项目的运行流程如下：

1. 启动 `turtlesim_node`
2. 启动 `turtle_control` 节点
3. 服务端创建 `patrol` 服务，并持续监听 `/turtle1/pose`
4. 启动 `patrol_client` 节点
5. 客户端每隔 10 秒生成一个随机目标点
6. 客户端向 `patrol` 服务发送请求
7. 服务端判断坐标是否合法
8. 若合法，则更新目标坐标
9. 控制节点根据当前位姿与目标点计算速度
10. 小海龟朝目标点移动

---

## 控制思路

服务端采用了一个很基础的目标点控制方法。

### 1. 获取当前位置

通过订阅 `/turtle1/pose` 获取当前：

* `x`
* `y`
* `theta`

### 2. 计算距离误差

```text
distance = sqrt((target_x - current_x)^2 + (target_y - current_y)^2)
```

### 3. 计算角度误差

```text
angle = atan2(target_y - current_y, target_x - current_x) - theta
```

### 4. 控制策略

* 当距离大于 `0.1` 时，才开始运动
* 当角度误差大于 `0.2` 时，优先旋转
* 当角度误差较小时，开始前进

### 5. 速度限制

线速度使用：

```text
linear.x = k * distance
```

并设置了最大线速度限制：

```text
max_speed = 3.0
```

---

## 目标点合法性规则

服务端只接受满足条件的目标点。根据当前源码，判断逻辑等价于：

* `target_x != 0`
* `target_y != 0`
* `target_x < 12.0`
* `target_y < 12.0`

因此：

* 坐标为 `0` 的点不会通过
* 大于等于 `12.0` 的点不会通过

而客户端随机生成的是 `0 ~ 14`，所以有些请求会被服务端拒绝，这是代码设计上的正常现象。

---

## 依赖环境

建议环境：

* Ubuntu 22.04
* ROS 2 Humble
* `rclcpp`
* `geometry_msgs`
* `turtlesim`
* `colcon`

---

## 编译方法

在工作空间根目录执行：

```bash
colcon build --packages-select chapt4_turtle_interfaces demo_cpp_service
```

编译完成后加载环境：

```bash
source install/setup.bash
```

---

## 运行方法

### 方法一：使用 launch 一键启动

```bash
source install/setup.bash
ros2 launch demo_cpp_service demo.launch.py
```

指定背景绿色通道：

```bash
ros2 launch demo_cpp_service demo.launch.py launch_arg_bg:=0
```

或者：

```bash
ros2 launch demo_cpp_service demo.launch.py launch_arg_bg:=255
```

---

### 方法二：手动分终端启动

#### 终端 1：启动 turtlesim

```bash
ros2 run turtlesim turtlesim_node
```

#### 终端 2：启动服务端控制节点

```bash
source install/setup.bash
ros2 run demo_cpp_service turtle_control
```

#### 终端 3：启动客户端节点

```bash
source install/setup.bash
ros2 run demo_cpp_service patrol_client
```

运行后，客户端会每隔 10 秒发送一个随机目标点，小海龟会尝试朝该目标点移动。

---

## 可执行文件

`demo_cpp_service` 当前会编译出两个可执行程序：

* `turtle_control`
* `patrol_client`

---

## 代码特点

这个项目有几个比较明显的特点：

### 1. Service 与运动控制结合

它不是单纯地“收到消息就动”，而是通过 Service 先更新目标点，再通过订阅/发布实现持续控制。

### 2. 结构清晰

接口包与功能包是分开的：

* `chapt4_turtle_interfaces` 只负责接口
* `demo_cpp_service` 只负责功能实现

这比较符合 ROS 2 工程化的组织方式。

### 3. 增加了 launch 启动方式

除了原来的手动多终端运行，现在也可以通过 launch 文件一键启动整个系统，并通过参数修改 `turtlesim` 的背景绿色通道。

### 4. 控制逻辑简单直观

代码重点是练习通信与控制流程，因此控制器设计得比较基础，便于理解。

---

## 已知限制

根据当前源码，这个项目还有一些比较明显的限制：

### 1. 角度误差没有做归一化

当前直接计算：

```text
atan2(...) - theta
```

没有把角度归一化到 `[-pi, pi]`，因此在某些角度情况下可能不是最优转向。

### 2. 旋转速度使用了绝对值

当角度误差较大时，代码里使用的是：

```cpp
msg.angular.z = fabs(angle);
```

这会丢失正负号，意味着转向方向逻辑并不完整，严格来说还有改进空间。

### 3. 目标点生成范围和合法范围不一致

客户端生成 `0 ~ 14`，服务端只接受小于 `12.0` 且非 0 的点，因此会出现部分请求天然失败。

### 4. 当前更接近“随机导航”而不是“固定巡逻”

它没有预设多个巡逻点，也没有闭环路线，只是不断接收新的随机目标点。

---

## 可改进方向

后续可以继续扩展：

* 对角度误差做归一化处理
* 保留角速度符号，让海龟能正确选择左转或右转
* 优化随机坐标生成范围，使其与合法范围一致
* 支持多个巡逻点
* 实现固定路线巡逻
* 增加目标到达提示
* 增加更清晰的状态反馈
* 使用更多 launch 参数，例如背景 RGB、定时周期、速度参数等

---

## 学习价值

通过这个项目，可以练习：

* ROS 2 workspace 与 package 的组织方式
* 自定义 `.srv` 接口的创建与使用
* ROS 2 C++ Service 客户端与服务端开发
* `publisher / subscriber` 的配合使用
* turtlesim 的基础控制
* launch 文件编写与参数传递
* 从“请求目标”到“闭环移动”的基本思路

---

## License

Apache-2.0
