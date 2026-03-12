#ifndef CODROID_DEFINE_H
#define CODROID_DEFINE_H

#include <string>
#include <nlohmann/json.hpp>
#include "CodroidExport.h"
#include <stdexcept>
#include <functional>

namespace Codroid {
    using json = nlohmann::json;

    // 通用响应结构体
    struct Response {
        int id;       // 请求ID
        std::string ty;       // 请求类型
        json db;              // 返回数据
        std::string error_msg;// 错误信息
    };

    /**
     * @brief Codroid SDK 专用异常类
     */
    class CodroidException : public std::runtime_error {
    public:
        explicit CodroidException(const std::string& message) 
            : std::runtime_error(message) {}
    };

    /**
     * @brief RS485 Parity / RS485 校验位
     */
    enum class RS485Parity : int {
        None = 0, ///< No parity / 无校验
        Odd  = 1, ///< Odd parity / 奇校验
        Even = 2  ///< Even parity / 偶校验
    };

    /**
     * @brief RS485 Stop Bits / RS485 停止位
     */
    enum class RS485StopBits : int {
        One = 1, ///< 1 stop bit / 1位停止位
        Two = 2  ///< 2 stop bits / 2位停止位
    };

    // --- 枚举定义 ---
    enum class CoorType { Tool, User };
    NLOHMANN_JSON_SERIALIZE_ENUM(CoorType, {
        {CoorType::Tool, "tool"}, {CoorType::User, "user"}
    })

    enum class MoveType { movJ, movL, movC, movCircle };
    NLOHMANN_JSON_SERIALIZE_ENUM(MoveType, {
        {MoveType::movJ, "movJ"}, {MoveType::movL, "movL"},
        {MoveType::movC, "movC"}, {MoveType::movCircle, "movCircle"}
    })

    /**
     * @brief 相对位姿计算参数
     */
    struct RelativePoseParams {
        std::vector<double> pos;      ///< [Required] Current position (Cartesian) / [必填] 当前末端TCP坐标 [x,y,z,a,b,c]
        std::vector<double> offset;   ///< [Required] Desired offset in Cartesian coordinates / [必填] 偏移量 [x,y,z,a,b,c]
        
        // --- 以下为可选参数，使用空数组作为“未设置”的标志 ---
        CoorType coorType = CoorType::Tool; ///< [Optional] Coordinate system type for the offset (Tool or User) / [可选] 坐标系类型
        std::vector<double> posCoor;  ///< [Optional] Coordinate of the current position / [可选] 当前末端TCP坐标系，默认世界坐标系
        std::vector<double> coor;     ///< [Optional] coorType is valid when set to user; offset coordinate system is the default, world coordinate system / [可选] coorType为user时有效，偏移坐标系，默认世界坐标系

        // 辅助构造函数，方便快速创建最简单的请求
        RelativePoseParams(const std::vector<double>& p, const std::vector<double>& o, CoorType type)
            : pos(p), offset(o), coorType(type) {}
            
        RelativePoseParams() = default;
    };


    /**
     * @brief Global variable information / 全局变量信息结构
     */
    struct Variable {
        std::string val;  ///< Variable value (JSON string format) / 变量值 (JSON字符串格式)
        std::string nm;   ///< Variable remark/note / 变量备注

        /**
         * @brief 构造函数
         * @param[in] value 变量值，支持直接传入 int, double, string, json 对象等
         * @param[in] note 变量备注，可选
         */
        template<typename T>
        Variable(const T& value, const std::string& note = "") : nm(note) {
            if constexpr (std::is_same_v<T, std::string>) {
                // 如果是字符串，按照示例某些情况可能需要额外转义，
                // 但通常直接赋值或 dump 即可
                val = value; 
            } else {
                // 使用 json 序列化功能将 数组、对象、数字 转为字符串
                nlohmann::json j = value;
                val = j.dump();
            }
        }
        
        // 默认构造函数
        Variable() = default;
    };
    
    /**
     * @brief 正解参数结构体 (Forward Kinematics Params)
     */
    struct FKParams {
        std::vector<double> jp;      ///< [必填] 关节角 [j1...j6], 单位: deg
        std::vector<double> coor;    ///< [可选] 用户坐标系, 不传则不处理
        std::vector<double> tool;    ///< [可选] 工具坐标系, 不传则不处理
        std::vector<double> ep;      ///< [可选] 附加轴位置

        // 构造函数，方便快速传入必填项
        explicit FKParams(const std::vector<double>& jointPos) : jp(jointPos) {}
        FKParams() = default;
    };

    /**
     * @brief 逆解参数结构体 (Inverse Kinematics Params)
     */
    struct IKParams {
        std::vector<double> cp;      ///< [必填] 笛卡尔末端位置, 单位: mm, deg
        std::vector<double> rj;      ///< [可选] 参考关节角, 默认 [20,20,20,20,20,20]
        std::vector<double> ep;      ///< [可选] 附加轴位置

        // 构造函数
        explicit IKParams(const std::vector<double>& cartesianPos) : cp(cartesianPos) {}
        IKParams() = default;
    };

    /**
     * @brief Jog Mode / 点动模式
     */
    enum class JogMode : int {
        Joint = 1, ///< Joint jog / 关节点动
        Line  = 2  ///< Line (Cartesian) jog / 直线点动
    };

    /**
     * @brief Jog Coordinate Type / 点动坐标系类型
     */
    enum class JogCoorType : int {
        User = 0, ///< User coordinate system / 用户坐标系
        Tool = 1  ///< Tool coordinate system / 工具坐标系
    };

    /**
     * @brief Jog Parameters / 点动参数结构体
     */
    struct JogParams {
        JogMode mode = JogMode::Line;      ///< 1: Joint, 2: Line / 1：关节点动 2：直线点动
        double speed = 0.0;               ///< Speed range -1~1 / 速度，取值范围-1~1
        int index = 1;                    ///< Joint 1-6 or XYZABC 1-6 / 关节序号1~6 或 空间轴序号1~6(xyzabc)
        JogCoorType coorType = JogCoorType::User; ///< 0: User, 1: Tool / 0：用户坐标系，1：工具坐标系
        int coorId = 1;                   ///< User coordinate ID / 用户坐标系id

        JogParams() = default;
        
        /**
         * @brief 快速构造函数
         */
        JogParams(JogMode m, double s, int idx, JogCoorType ct = JogCoorType::User, int cid = 1)
            : mode(m), speed(s), index(idx), coorType(ct), coorId(cid) {}
    };

    // --- JSON 映射配置 (确保 int 枚举正确序列化) ---
    NLOHMANN_JSON_SERIALIZE_ENUM(JogMode, {
        {JogMode::Joint, 1},
        {JogMode::Line, 2}
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(JogCoorType, {
        {JogCoorType::User, 0},
        {JogCoorType::Tool, 1}
    })

    /**
     * @brief MoveTo Type / 运动类型
     */
    enum class MoveToType : int {
        Home = 0,           ///< Home position / Home 位置
        Safe = 1,           ///< Safe position / 安全位置
        Candle = 2,         ///< Candle position / 蜡烛位
        Packing = 3,        ///< Packing position / 打包位
        Joint = 4,          ///< Joint planning to position / 关节规划到指定位置
        Line = 5,          ///< Line planning to position / 直线规划到指定位置
        ResumePoint = 6     ///< Program resume point / 程序恢复点
    };

    /**
     * @brief MoveTo Target Position / 运动目标位置
     * 可以仅提供 cp，或仅提供 jp，或者两者都提供（由机器人端决定优先级）
     */
    struct MoveToTarget {
        std::vector<double> cp; ///< [可选] 末端位置 [x,y,z,a,b,c]
        std::vector<double> jp; ///< [可选] 关节位置 [j1..j6]
        // ep 字段在 SDK 内部固定处理，无需手动赋值

        MoveToTarget() = default;

        // 辅助方法：快速创建目标
        static MoveToTarget Joint(const std::vector<double>& j) {
            MoveToTarget t; t.jp = j; return t;
        }
        static MoveToTarget Cartesian(const std::vector<double>& c) {
            MoveToTarget t; t.cp = c; return t;
        }
    };

    /**
     * @brief MoveTo Parameters / 运动参数
     */
    struct MoveToParams {
        MoveToType type = MoveToType::Home;
        MoveToTarget target; ///< Optional, used only for type 4 or 5 / 可选，仅用于类型 4 或 5

        MoveToParams() = default;
        // 预定义位置构造 (Home, Safe 等)
        MoveToParams(MoveToType t) : type(t) {}
        // 规划位置构造 (Joint/Line Planning)
        MoveToParams(MoveToType t, const MoveToTarget& tgt) : type(t), target(tgt) {}
    };

    // JSON 映射
    NLOHMANN_JSON_SERIALIZE_ENUM(MoveToType, {
        {MoveToType::Home, 0},
        {MoveToType::Safe, 1},
        {MoveToType::Candle, 2},
        {MoveToType::Packing, 3},
        {MoveToType::Joint, 4},
        {MoveToType::Line, 5},
        {MoveToType::ResumePoint, 6}
    })

    /**
     * @brief Move point / 运动点结构体
     * 适用于 move 指令，包含更丰富的运动参数和目标定义
     */
    struct MovePoint {
        std::vector<double> jp; ///< jointposition / 关节角
        std::vector<double> cp; ///< cartesian position / 笛卡尔位置
        std::vector<double> rj; ///< reference position / 参考位置
        std::vector<double> ep; ///< extra axes / 附加轴

        MovePoint() = default;
        static MovePoint Joint(const std::vector<double>& j) { MovePoint p; p.jp = j; return p; }
        static MovePoint Cartesian(const std::vector<double>& c) { MovePoint p; p.cp = c; return p; }
    };

    /**
     * @brief Move Instruction / 运动指令结构体
     */
    struct MoveInstruction {
        MoveType type = MoveType::movJ; ///< movetype: movJ, movL, movC, movCircle / 运动类型：movJ、movL、movC、movCircle
        double speed = 60.0;            ///< speed, unit mm/s for linear, deg/s for joint / 速度，线性运动单位mm/s，关节运动单位deg/s
        double acc = 150.0;             ///< acceleration, unit mm/s² for linear, deg/s² for joint / 加速度，线性运动单位mm/s²，关节运动单位deg/s²
        double blend = -1.0;            ///< blend radius for movL and movC, unit mm for linear, deg for joint; <0 means no blending / movL和movC的blend半径，线性运动单位mm，关节运动单位deg；<0表示不使用blend
        double relativeBlend = -1.0;    ///< relative blend radius (0~1) for movL and movC; <0 means absolute blend radius / movL和movC的相对blend半径（0~1）；<0表示使用绝对blend半径
        int circleNum = 1;              ///< circle number for movC and movCircle / movC和movCircle的圈数
        
        MovePoint targetPoint;          ///< target point for the movement / 运动目标点，包含关节角和笛卡尔位置等信息
        MovePoint middlePoint;          ///< middle point for movC / movC的中间点，包含关节角和笛卡尔位置等信息
        
        std::vector<double> coor;       ///< optional coordinate system for the target point, default is world coordinate system / 目标点的可选坐标系，默认是世界坐标系
        std::vector<double> tool;       ///< optional tool coordinate system for the target point / 目标点的可选工具坐标系
    };

    // 回调函数定义：参数1是主题名称(ty)，参数2是具体数据内容(db)
    using TopicCallback = std::function<void(const std::string&, const nlohmann::json&)>;

    // 机器人姿态快照结构体
    struct RobotPostureSnapshot {
        std::vector<double> joint;   ///< 关节角 [j1..j6] (单位: 弧度)
        std::vector<double> cart;    ///< 笛卡尔坐标 [x,y,z,a,b,c] (单位: mm, 弧度)
        
        RobotPostureSnapshot() : joint(6, 0.0), cart(6, 0.0) {}
    };

    /**
     * @brief IO 信息结构体
     */
    struct IOInfo {
        std::string type; ///< "DI", "DO", "AI", "AO"
        int port;         ///< 端口号
        double value;     ///< IO 的值 (查询结果)

        IOInfo() : port(0), value(0.0) {}
        IOInfo(const std::string& t, int p) : type(t), port(p), value(0.0) {}
    };

    enum class IOType { DI, DO, AI, AO };

    /**
     * @brief 寄存器键值对
     */
    struct RegisterInfo {
        int address;
        double value;

        // 默认构造函数
        RegisterInfo() : address(0), value(0.0) {}

        // 带参数的构造函数（解决列表初始化匹配问题）
        RegisterInfo(int addr, double val) : address(addr), value(val) {}
    };

    /**
     * @brief 扩展数组支持的数据类型
     */
    enum class ExtendArrayType {
        Bool,
        UInt8,
        Int8,
        UInt16,
        Int16,
        UInt32,
        Int32,
        Float32
    };

    // 映射枚举到字符串，用于 JSON 序列化
    NLOHMANN_JSON_SERIALIZE_ENUM(ExtendArrayType, {
        {ExtendArrayType::Bool, "Bool"},
        {ExtendArrayType::UInt8, "UInt8"},
        {ExtendArrayType::Int8, "Int8"},
        {ExtendArrayType::UInt16, "UInt16"},
        {ExtendArrayType::Int16, "Int16"},
        {ExtendArrayType::UInt32, "UInt32"},
        {ExtendArrayType::Int32, "Int32"},
        {ExtendArrayType::Float32, "Float32"}
    })

    // 15.4 机器人状态
    struct RobotStatus {
        int mode;            // 0=手动; 1=自动; 2=远程
        int state;           // 0=未使能; 1=使能中; 2=空闲; 3=点动中; 4=RunTo; 5=拖动中
        int isMoving;        // 0: 停止, 1: 运动
        double moveRate;        
        double manualMoveRate;
        int recoveryState;
        bool isSimulation;
        int teachingPendant;
        int rescueFlag;
        int modeSwitch;
        int ToolId;
        int PayloadId;
        int CoordinateId;
        int defaultToolId;
        int defaultPayloadId;
        int defaultUserCoorId;
        std::string type;
        std::string stateName;
        long long timestamp;
    };

    // --- 对应协议的数据结构 ---

    struct RobotStatusPush {
        int mode;           // 0=手动; 1=自动; 2=远程
        int state;          // 0=未使能; 1=使能中; 2=空闲; 3=点动中; 4=RunTo; 5=拖动中
        bool isMoving;      // 是否正在运动
        double moveRate;    // 自动倍率
        std::string type;   // 机器人型号
        std::string stateName;
    };

    struct RobotPosturePush {
        std::vector<double> joint; // 关节角 (deg)
        struct {
            double x, y, z, a, b, c;
        } cart; // 笛卡尔坐标
    };

    struct ProjectStatePush {
        std::string id;
        int state;       // 0:空闲, 1:加载, 2:运行, 3:暂停
        bool isStep;
        std::map<std::string, int> scriptLines; // 脚本ID -> 行号
    };

}

#endif