#include <iostream>
#include <iomanip> // 用于美化输出
#include <thread>
#include <chrono>
#include "Codroid/CodroidControlInterface.h"

// 辅助函数：打印响应结果
void printResponse(const std::string& action, const Codroid::Response& resp) {
    std::cout << "----- " << action << " -----" << std::endl;
    std::cout << "Request ID: " << resp.id << std::endl;
    std::cout << "Type:       " << resp.ty << std::endl;
    
    if (resp.error_msg.empty()) {
        std::cout << "Status:     [SUCCESS]" << std::endl;
        std::cout << "Data:       " << resp.db.dump(4) << std::endl;
    } else {
        std::cout << "Status:     [FAILED]" << std::endl;
        std::cout << "Error:      " << resp.error_msg << std::endl;
    }
    std::cout << "-----------------------" << std::endl << std::endl;
}

int main() {
    // 1. 实例化控制类
    Codroid::CodroidControlInterface robot;

    // 2. 配置机械臂 IP 和 端口 (请根据实际情况修改)
    std::string robot_ip = "192.168.1.136"; 
    int robot_port = 9001;  // 默认端口通常是 9001，除非你修改过服务器设置

    std::cout << "Connecting to robot at " << robot_ip << ":" << robot_port << "..." << std::endl;

    // 3. 尝试连接
    if (!robot.connect(robot_ip)) {
        std::cerr << "Critical Error: Could not connect to the robot!" << std::endl;
        return -1;
    }
    std::cout << "Connected successfully!" << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // 调用上电接口
    std::cout << "Sending SwitchOn command..." << std::endl;
    auto resOn = robot.switchOn(101);
    printResponse("Switch On", resOn);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 测试 RS485 写入接口
    // 准备要发送的十六进制数据，例如：0x02, 0x0A
    std::vector<uint8_t> buffer = {0x02, 0x0A, 0x45, 0x21};
    // 初始化
    auto resRS485 = robot.RS485init(115200);
    if (resRS485.error_msg.empty()) {
        std::cout << "RS485 Init Success!" << std::endl;
    } else {
        std::cerr << "RS485 Init Failed: " << resRS485.error_msg << std::endl;
    }

    auto resRS485write = robot.RS485write(buffer, 102);

    if (resRS485write.error_msg.empty()) {
        std::cout << "RS485 Write Success!" << std::endl;
    } else {
        std::cerr << "RS485 Write Failed: " << resRS485write.error_msg << std::endl;
    }

    

    // 调用下电接口
    std::cout << "Sending SwitchOff command..." << std::endl;
    auto resOff = robot.switchOff(102);
    printResponse("Switch Off", resOff);

    // 断开连接 (析构函数也会自动处理，但手动断开是好习惯)
    robot.disconnect();
    std::cout << "Connection closed." << std::endl;

    return 0;
}