#include "Codroid/CodroidSubscriber.h"
#include <iostream>

namespace Codroid {

CodroidSubscriber::CodroidSubscriber() 
    : socket_(std::make_unique<asio::ip::tcp::socket>(io_context_)) {}

CodroidSubscriber::~CodroidSubscriber() {
    disconnect();
}

bool CodroidSubscriber::connect(const std::string& ip, int port) {
    try {
        asio::ip::tcp::resolver resolver(io_context_);
        asio::connect(*socket_, resolver.resolve(ip, std::to_string(port)));
        socket_->set_option(asio::ip::tcp::no_delay(true));

        running_ = true;
        thread_ = std::thread(&CodroidSubscriber::runLoop, this);
        return true;
    } catch (std::exception& e) {
        std::cerr << "Subscriber connect error: " << e.what() << std::endl;
        return false;
    }
}

void CodroidSubscriber::disconnect() {
    running_ = false;
    if (socket_ && socket_->is_open()) {
        socket_->close();
    }
    if (thread_.joinable()) thread_.join();
}

bool CodroidSubscriber::subscribe(const std::string& topic, int cycle) {
    try {
        json req;
        req["ty"] = "publish/" + topic;
        req["tc"] = cycle;
        std::string s = req.dump() + "\n";
        asio::write(*socket_, asio::buffer(s));
        return true;
    } catch (...) { return false; }
}

void CodroidSubscriber::runLoop() {
    while (running_) {
        std::string raw = receiveOneRaw();
        if (raw.empty()) break;

        try {
            json j = json::parse(raw);
            handleMessage(j);
        } catch (...) {}
    }
}

void CodroidSubscriber::handleMessage(const json& j) {
    std::string ty = j.value("ty", "");
    if (!j.contains("db") || j["db"].is_null()) return;
    const auto& db = j["db"];

    // 15.4 机器人状态
    if (ty == "publish/RobotStatus" && onStatus_) {
        RobotStatusPush s;
        s.mode = db.value("mode", 0);
        s.state = db.value("state", 0);
        s.isMoving = (db.value("isMoving", 0) != 0);
        s.moveRate = db.value("moveRate", 0.0);
        s.type = db.value("type", "");
        s.stateName = db.value("stateName", "");
        onStatus_(s);
    }
    // 15.5 机器人位姿
    else if (ty == "publish/RobotPosture" && onPosture_) {
        RobotPosturePush p;
        p.joint = db.value("joint", std::vector<double>{});
        if (db.contains("end")) {
            p.cart.x = db["end"].value("x", 0.0);
            p.cart.y = db["end"].value("y", 0.0);
            p.cart.z = db["end"].value("z", 0.0);
            p.cart.a = db["end"].value("a", 0.0);
            p.cart.b = db["end"].value("b", 0.0);
            p.cart.c = db["end"].value("c", 0.0);
        }
        onPosture_(p);
    }
    // 15.3 变量更新
    else if (ty == "publish/VarUpdate" && onVar_) {
        std::map<std::string, std::string> vars;
        for (auto it = db.begin(); it != db.end(); ++it) {
            vars[it.key()] = it.value().get<std::string>();
        }
        onVar_(vars);
    }
    // 15.7/15.8 日志与错误
    else if ((ty == "publish/Log" || ty == "publish/Error") && onLog_) {
        for (const auto& logEntry : db) {
            if (logEntry.is_array() && logEntry.size() >= 4) {
                // logEntry[0]是等级, logEntry[3]是内容
                onLog_(logEntry[3].get<std::string>(), logEntry[0].get<int>());
            }
        }
    }
}

std::string CodroidSubscriber::receiveOneRaw() {
    char chunk[1024];
    while (running_) {
        // 尝试从 buffer 中找完整包
        size_t braces = 0;
        int start = -1;
        for (int i = 0; i < (int)buffer_.length(); ++i) {
            if (buffer_[i] == '{') {
                if (start == -1) start = i;
                braces++;
            } else if (buffer_[i] == '}') {
                if (start != -1) {
                    braces--;
                    if (braces == 0) {
                        std::string res = buffer_.substr(start, i - start + 1);
                        buffer_.erase(0, i + 1);
                        return res;
                    }
                }
            }
        }
        // 没找到，读 Socket
        asio::error_code ec;
        size_t len = socket_->read_some(asio::buffer(chunk, sizeof(chunk)), ec);
        if (ec) return "";
        buffer_.append(chunk, len);
    }
    return "";
}

} // namespace Codroid