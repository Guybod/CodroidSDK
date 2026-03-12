#ifndef CODROID_SUBSCRIBER_H
#define CODROID_SUBSCRIBER_H

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <map>
#include <mutex>
#include "CodroidDefine.h"

namespace Codroid {

using json = nlohmann::json;

// 这里的回调定义可以根据需要增加
using StatusCallback = std::function<void(const RobotStatusPush&)>;
using PostureCallback = std::function<void(const RobotPosturePush&)>;
using VarCallback    = std::function<void(const std::map<std::string, std::string>&)>;
using LogCallback    = std::function<void(const std::string& msg, int level)>;

class CODROID_API CodroidSubscriber {
public:
    CodroidSubscriber();
    ~CodroidSubscriber();

    // 1. 连接与断开
    bool connect(const std::string& ip, int port = 9001);
    void disconnect();

    // 2. 订阅特定主题 (发送 15.1 协议指令)
    bool subscribe(const std::string& topic, int cycle = 0);

    // 3. 设置回调函数
    void setStatusCallback(StatusCallback cb) { onStatus_ = cb; }
    void setPostureCallback(PostureCallback cb) { onPosture_ = cb; }
    void setVarCallback(VarCallback cb) { onVar_ = cb; }
    void setLogCallback(LogCallback cb) { onLog_ = cb; }

private:
    void runLoop();
    void handleMessage(const json& j);
    std::string receiveOneRaw();

    asio::io_context io_context_;
    std::unique_ptr<asio::ip::tcp::socket> socket_;
    
    std::string buffer_;
    std::thread thread_;
    std::atomic<bool> running_{false};

    // 回调对象
    StatusCallback onStatus_;
    PostureCallback onPosture_;
    VarCallback onVar_;
    LogCallback onLog_;
};

} // namespace Codroid

#endif