#ifndef CODROID_CONTROL_INTERFACE_H
#define CODROID_CONTROL_INTERFACE_H

#include "CodroidDefine.h"
#include <unordered_set>
#include <asio.hpp>
#include <memory>
#include <string>

namespace Codroid {

class CODROID_API CodroidControlInterface {
public:
    CodroidControlInterface();
    ~CodroidControlInterface();

    // 禁止拷贝，防止 Socket 管理混乱
    CodroidControlInterface(const CodroidControlInterface&) = delete;
    CodroidControlInterface& operator=(const CodroidControlInterface&) = delete;

    // 连接机械臂
    bool connect(const std::string& ip, int port = 9001);
    void disconnect();

    // 发送通用指令接口
    Response sendCommand(const std::string& type, const json& data, int id);

    // --- 2.1 运行脚本 ---
    Response runScript(const std::string& script, int id = 1);

    // --- 2.2 进入远程脚本模式 ---
    Response enterRemoteScriptMode(int id = 1);

    // --- 2.3 运行工程(工程ID) ---
    Response runProject(const std::string& projectid, int id = 1);

    // --- 2.4 运行工程(工程索引) ---
    Response runProjectByIndex(int index, int id = 1);

    // --- 2.5 单步运行 ---
    Response runStep(const std::string& projectid, int id = 1);

    // --- 2.6 暂停工程 ---
    Response pauseProject(int id = 1);

    // --- 2.7 恢复工程 ---
    Response resumeProject(int id = 1);

    // --- 2.8 停止工程 ---
    Response stopProject(int id = 1);

    // --- 2.13 设置启动行 ---
    Response setStartLine(int startline, int id = 1);
    
    // --- 2.14 清除启动行 ---
    Response clearStartLine(int id = 1);

    // --- 3.3 获取全局变量列表 ---
    Response getGlobalVars(int id = 1);

    // --- 3.4 保存全局变量列表 ---
    Response saveGlobalVars(const std::map<std::string, Variable>& vars, int id = 1);

    // --- 3.5 删除全局变量 ---
    Response removeGlobalVars(const std::vector<std::string>& varNames, int id = 1);

    // --- 4.1 工程变量接口 ---
    Response getProjectVar(int id = 1);

    // --- 5.1 初始化RS485 ---
    Response RS485init(int baudrate, RS485StopBits stopBit = RS485StopBits::One, int dataBit = 8, RS485Parity parity = RS485Parity::None, int id = 1);

    // --- 5.2 RS485 flush ---
    Response RS485flush(int id = 1);

    // --- 5.3 RS485 read ---
    Response RS485read(int length,int timeout = 5000, int id = 1);

    // --- 5.4 RS485 write ---
    Response RS485write(const std::vector<uint8_t>& data, int id = 1);

    // --- 10.1 正解接口 ---
    std::vector<double> forwardKinematics(const FKParams& params, int id = 1);

    // --- 10.2 逆解接口 ---
    std::vector<double> inverseKinematics(const IKParams& params, int id = 1);
    
    // --- 10.3 笛卡尔坐标偏移计算接口 ---
    std::vector<double> calculateRelativePose(const RelativePoseParams& params, int id = 1);

    // --- 11.1 点动 ---
    Response jog(const JogParams& params, int id = 1);

    // --- 11.2 停止点动 ---
    Response stopJog(int id = 1);

    // --- 11.3 点动心跳 ---
    Response jogHeartbeat(int id = 1);

    // --- 11.4 moveTo ---
    Response moveTo(const MoveToParams& params, int id = 1);

    // --- 11.5 moveTo心跳 ---
    Response moveToHeartbeat(int id = 1);

    // --- 11.6 设置手动运动倍率 ---
    Response setManualSpeedRate(int speed, int id = 1);

    // --- 11.7 设置自动运动倍率 ---
    Response setAutoSpeedRate(int speed, int id = 1);

    // --- 11.8 运动指令 ---
    // --- 核心运动接口 ---
    Response move(const std::vector<MoveInstruction>& path, int id = 1);
    
    // --- movJ 重载 ---
    Response movJ(const MoveInstruction& inst, int id = 1);
    Response movJ(const std::vector<double>& jp, double speed, double acc, int id = 1);

    // --- movL 重载 ---
    Response movL(const MoveInstruction& inst, int id = 1);
    Response movL(const std::vector<double>& cp, double speed, double acc, 
                  const std::vector<double>& coor = {}, const std::vector<double>& tool = {}, int id = 1);
    
    // --- movC 重载 ---
    Response movC(const MoveInstruction& inst, int id = 1);

    // --- movCircle (整圆运动) ---
    Response movCircle(const MoveInstruction& inst, int id = 1);

    // --- 11.9 暂停运动 ---
    Response pauseMove(int id = 1);

    // --- 11.10 恢复运动 ---
    Response resumeMove(int id = 1);

    // --- 11.11 停止运动 ---
    Response stopMove(int id = 1);

    // --- 12.1 上使能 ---
    Response switchOn(int id = 1);

    // --- 12.2 下使能 ---
    Response switchOff(int id = 1);

    // --- 12.3 进入手动模式
    Response toManual(int id = 1);

    // --- 12.4 进入自动模式
    Response toAuto(int id = 1);

    // --- 12.5 进入远程模式
    Response toRemote(int id = 1);

    // --- 12.7 进入仿真模式 ---
    Response toSimulation(int id = 1);

    // --- 12.8 进入实机模式 ---
    Response toActual(int id = 1);

    // --- 12.9 进入拖拽模式 ---
    Response startDrag(int id = 1);

    // --- 12.10 退出拖拽模式 ---
    Response stopDrag(int id = 1);

    // --- 12.11 清除错误 ---
    Response clearError(int id = 1);

    // --- 13.1 获取IO状态 ---
    std::vector<IOInfo> getIOValues(const std::vector<IOInfo>& queryList, int id = 1);
    int getDI(int port, int id = 1);
    int getDO(int port, int id = 1);
    double getAI(int port, int id = 1);
    double getAO(int port, int id = 1);

    // --- 13.2 设置IO状态 ---
    Response setIOValue(const std::string& type, int port, double value, int id = 1);
    Response setDO(int port, int value, int id = 1);
    Response setAO(int port, double value, int id = 1);

    // --- 14.1 获取寄存器值 ---
    std::vector<RegisterInfo> getRegisterValues(const std::vector<int>& addresses, int id = 1);
    double getRegisterValue(const int address, int id = 1);

    // --- 14.2 设置寄存器值 ---
    Response setRegisterValue(int address, double value, int id = 1);
    
    // --- 14.3 设置扩展数组数据类型 ---
    Response setExtendArrayType(int index, ExtendArrayType type, int id = 1);
    
    // --- 14.4 删除扩展数组索引 ---
    Response removeExtendArray(int index, int id = 1);

    // --- 17.2 开启数据推送流 ---
    Response startDataPush(const std::string& ip, int port,int duration, int id = 1);

    // --- 17.3 关闭数据推送流 ---
    Response stopDataPush(int id = 1);

    // --- 17.4 开启实时控制 ---
    Response startControl(int duration, int startBuffer = 5, int filterTypeint = 1, int id = 1);

    // --- 17.5 关闭实时控制 ---
    Response stopControl(int id = 1);
    
    // --- 19.1 设置碰撞灵敏度 ---
    Response setCollisionSensitivity(int level, int id = 1);

    // --- 19.2 设置负载参数 ---
    Response setPayload(int payloadId, int id = 1);

private:
    
    asio::io_context io_context_;

    std::unique_ptr<asio::ip::tcp::socket> cmd_socket_;
    std::mutex cmd_mtx_; // 指令通道加锁，保证请求-响应不交叉

    std::string cmd_buffer_; // 指令通道的粘包缓冲区
    // 内部辅助：接收完整 JSON
    std::string receiveRaw(asio::ip::tcp::socket& socket, std::string& sticky_buffer);

    // 内部校验函数
    bool isValidVariableName(const std::string& name, std::string& outError);

    // 关键字集合 (Lua 风格)
    const std::unordered_set<std::string> luaKeywords = {
        "and", "break", "do", "else", "elseif", "end","false", "for", "function", "goto", 
        "if", "in","local", "nil", "not", "or", "repeat", "return","then", "true", "until",
        "while", "table", "math","DO", "DOGroup", "DIO", "DIOGroup", "AO", "AIO","ModbusTCP",
        "setSpeedJ", "setAccJ", "setSpeedL", "setAccL", "setBlender","setMoveRate","getCoor",
        "getTool", "setCoor", "editCoor", "setTool", "editTool","setPayload","enableVibrationSuppression",
        "disableVibrationSuppression","setCollisionDetectionSensitivity","initComplianceControl",
        "enableComplianceControl","disableComplianceControl","forceControlZeroCalibrate",
        "setFilterPeriod","searchSuccessed","getJoint", "getTCP", "getCoor", "getTool",
        "aposToCpos","cposToApos", "cposToCpos","posOffset", "posTrans", "coorRel", "toolRel",
        "getJointTorque","getJointExternalTorque","createTray", "getTrayPos", "posInverse",
        "distance", "interPos","planeTrans","getTrajStart", "getTrajEnd", "arrayAdd", "arraySub",
        "coorTrans","movJ", "movL", "movC", "movCircle", "movLW", "movCW", "movTraj","setWeave",
        "weaveStart", "weaveEnd","setDO", "getDI", "getDO", "setDOGroup", "getDIGroup","getDOGroup",
        "setAO", "getAI", "getAO","getRegisterBool", "setRegisterBool", "getRegisterInt","setRegisterInt", 
        "getRegisterFloat", "setRegisterFloat","RS485init", "RS485flush", "RS485write", "RS485read",
        "readCoils", "readDiscreteInputs", "readHoldingRegisters","readInputRegisters","writeSingleCoil", 
        "writeSingleRegister", "writeMultipleCoils","writeMultipleRegisters","createSocketClient", 
        "connectSocketClient", "writeSocketClient","readSocketClient", "closeSocketClient","wait", 
        "waitCondition", "systemTime", "stopProject","pauseProject", "runScript", "pauseScript", 
        "resumeScript","stopScript", "callModule", "print","setInterruptInterval", "setInterruptCondition",
        "clearInterrupt","strcmp", "strToNumberArray", "arrayToStr","enableMultiWeld", "getCurSeam", 
        "isMultiWeldFinished","setMultiWeldOffset", "weldNextSeam", "resetMultiWeld","searchStart", 
        "setMasterFlag", "getOffsetValue", "search","searchEnd", "searchOffset", "searchOffsetEnd", 
        "searchError"
    };
    json packInstruction(const MoveInstruction& inst);
};

}

#endif