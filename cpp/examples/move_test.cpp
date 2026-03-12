#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include "Codroid/CodroidControlInterface.h"

// 高精度时间戳函数 [HH:MM:SS.ms]
std::string now() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);
    
    std::ostringstream oss;
    oss << "[" << std::put_time(&bt, "%H:%M:%S") << "." 
        << std::setfill('0') << std::setw(3) << ms.count() << "] ";
    return oss.str();
}

// 辅助函数：带时间戳的输出
void checkResponse(const std::string& action, const Codroid::Response& res) {
    if (!res.error_msg.empty()) {
        std::cerr << now() << action << " Failed: " << res.error_msg << std::endl;
    } else {
        std::cout << now() << action << " Success" << std::endl;
    }
}

void startMove(Codroid::CodroidControlInterface& robot) {
    try {
        // 打印当前位姿
        // --- 1. movJ 非阻塞 ---
        std::vector<double> homeJoints = {0.0, 0.0, 90.0, 0.0, 90.0, 0.0};
        std::cout << "\n" << now() << "1. Sending movJ non-blocking array mode" << std::endl;
        auto res1 = robot.movJ(homeJoints, 50, 100); 
        checkResponse("movJ Array", res1);
        
        // 打印当前位姿
        std::this_thread::sleep_for(std::chrono::seconds(5));
        // 打印当前位姿

        // --- 2. movL 非阻塞 ---
        std::cout << "\n" << now() << "2. Sending movL struct mode" << std::endl;
        Codroid::MoveInstruction inst;
        inst.type = Codroid::MoveType::movL;
        inst.targetPoint = Codroid::MovePoint::Cartesian({680.526, 191, 444.004, -179.999, 0, -90});
        inst.speed = 150.0;
        inst.acc = 300.0;
        auto res3 = robot.movL(inst);
        checkResponse("movL Struct", res3);

        // 打印当前位姿
        std::this_thread::sleep_for(std::chrono::seconds(5));
        // 打印当前位姿

    } 
    catch (const std::exception& e) {
        std::cerr << now() << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    Codroid::CodroidControlInterface robot;
    std::string robot_ip = "192.168.1.136"; 
    int robot_port = 9001;

    std::cout << now() << "Connecting to robot..." << std::endl;
    if (!robot.connect(robot_ip, robot_port)) {
        std::cerr << now() << "Failed to connect" << std::endl;
        return -1;
    }
    
    std::cout << now() << "Connected to robot successfully" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    
    startMove(robot);
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    robot.disconnect();
    std::cout << now() << "Disconnected. Test finished" << std::endl;
    
    return 0;
}