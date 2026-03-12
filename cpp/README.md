# Codroid SDK

Codroid SDK 是一个用于机械臂远程控制的跨平台 C++ 开发工具包。它通过 TCP 通信协议实现与机器人的交互，涵盖了运动控制（movJ/movL/movC）、IO 管理、寄存器操作、数据流订阅及实时控制等核心功能。

## 🌟 特点

- **开箱即用**：底层依赖库（Asio 和 nlohmann/json）已内置于 `third_party` 文件夹中，克隆仓库后无需额外安装系统依赖即可直接编译。
- **双通道架构**：支持独立的指令通道与状态订阅通道，确保高频运动指令下发与实时状态反馈互不干扰。
- **单位标准化**：SDK 内部已自动完成单位换算。
  - **长度单位**：毫米 (mm)
  - **角度单位**：角度 (deg)（机器人返回的弧度数据已在内部自动转换为角度，并保留 3 位小数）。
- **跨平台兼容**：完美支持 Linux (GCC) 与 Windows (Visual Studio / MSVC) 编译环境。

## 📁 项目结构

*   `include/Codroid/` - SDK 对外公开接口头文件。
*   `src/` - SDK 核心逻辑实现源代码。
*   `third_party/` - 内置依赖库（Asio Standalone, nlohmann/json）。
*   `examples/` - 功能测试示例（点动、路径运动、IO 测试、寄存器测试等）。
*   `build_linux.sh` - Linux 一键构建脚本。
*   `build_msvc.bat` - Windows MSVC 一键构建脚本。

## 🛠️ 构建指南

### 1. Linux (Ubuntu) 编译

在项目根目录下执行以下命令：

```bash
chmod +x build_linux.sh
./build_linux.sh
```
构建产物：

* 动态库：build_linux/libCodroid.so
* 示例程序：位于 build_linux/ 目录下的各个可执行文件（如 move_test）。

运行示例：
```bash
cd build_linux
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./move_test
```
### 2. Windows (Visual Studio / MSVC) 编译
在 Windows 环境下，确保已安装 CMake 和 Visual Studio 2019/2022，双击运行：
```Batch
build_msvc.bat
```
构建产物：
* build_msvc/Debug
* build_msvc/Release。

关键文件：Codroid.dll (运行时必须), Codroid.lib (编译链接必须)。

---
## 💻 如何在您的项目中使用
由于本 SDK 未采用 PIMPL 模式隐藏底层依赖，在您的第三方 C++ 项目中引入本 SDK 时，请务必进行以下配置：

1.包含目录 (Include Directories)

将以下两个路径添加到您的项目包含路径（Include Path）中：
* CodroidSDK/include
* CodroidSDK/third_party (由于头文件嵌套，必须包含此路径，否则找不到 asio.hpp)

2.预处理器定义 (Preprocessor Definitions)

必须添加以下宏定义：
* ASIO_STANDALONE (告诉 Asio 使用标准版而非 Boost 版)

3.Windows 环境额外配置

* 链接库：将 Codroid.lib 添加到链接器的附加依赖项。
* 字符编码：建议开启 /utf-8 编译选项，以确保中文注释不会引起编译语法错误。
* 运行时：程序运行时必须将 Codroid.dll 放置在与 .exe 文件相同的目录下。
  
## 🚀 快速上手示例
```c++
#include <iostream>
#include <vector>
#include "Codroid/CodroidControlInterface.h"

int main() {
    Codroid::CodroidControlInterface robot;

    // 1. 连接机器人 (默认端口 9001)
    if (robot.connect("192.168.1.136", 9001)) {
        std::cout << "Connected to Codroid Robot!" << std::endl;

        // 2. 获取当前笛卡尔位姿 (单位: mm, deg)
        // 内部已自动同步并跳过初始零位包
        std::vector<double> currentPos = robot.getCartesianPosition();
        std::cout << "Current X: " << currentPos[0] << " mm" << std::endl;

        // 3. 关节运动
        // 目标：关节角度 [j1..j6]
        std::vector<double> home = {0.0, 0.0, 90.0, 0.0, 90.0, 0.0};
        robot.movJ(home, 50.0, 100.0);

        // 4. 断开连接
        robot.disconnect();
    } else {
        std::cerr << "Connection failed!" << std::endl;
    }
    return 0;
}
```
## ⚠️ 注意事项
1. 状态同步延迟：受限于机器人端状态推送固定为 1 秒/次，Wait 系列阻塞函数在判断机器人停止时可能会存在最高 1 秒的自然同步延迟。
2. 空数组崩溃预防：SDK 内部实现了防御性编程，发送指令时会自动剔除空的 coor 或 tool 数组字段，以规避机器人后端处理空数组时的崩溃 Bug。
3. 多线程安全：sendCommand 内部已通过 std::mutex 加锁。支持在多线程环境下并发调用非阻塞运动指令，但建议在同一时刻只由一个线程控制机器人运动。
4. 单位验证：请注意，机器人推送的 joint 数据在 SDK 内部被视为角度（Degrees）。若您的机器人固件返回的是弧度，SDK 将按照 1rad ≈ 57.295deg 进行自动转换。