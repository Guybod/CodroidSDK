#!/bin/bash

# 开启严格模式：遇到错误立即停止
set -e

echo "=================================================="
echo "      Codroid SDK 自动化构建脚本 (物理集成版)        "
echo "=================================================="

# 1. 环境准备：物理复制第三方库 (解决跨平台迁移问题)
echo "[1/4] 正在将系统依赖库复制到项目内部 (third_party)..."

# 如果目录已存在，先清理，确保是干净的副本
rm -rf third_party
mkdir -p third_party

# 复制 asio.hpp
if [ -f "/usr/include/asio.hpp" ]; then
    echo "  -> 复制 asio.hpp"
    cp /usr/include/asio.hpp third_party/
else
    echo "  错误: 找不到 /usr/include/asio.hpp，请确保已安装 libasio-dev"
    exit 1
fi

# 复制 asio 目录
if [ -d "/usr/include/asio" ]; then
    echo "  -> 复制 asio 核心文件夹"
    cp -r /usr/include/asio third_party/
else
    echo "  错误: 找不到 /usr/include/asio 目录"
    exit 1
fi

# 复制 nlohmann 目录
if [ -d "/usr/include/nlohmann" ]; then
    echo "  -> 复制 nlohmann (json) 文件夹"
    cp -r /usr/include/nlohmann third_party/
else
    echo "  错误: 找不到 /usr/include/nlohmann 目录，请确保已安装 nlohmann-json3-dev"
    exit 1
fi

# 2. 生成 MinGW 交叉编译工具链文件 (供 Linux 终端使用)
echo "[2/4] 正在生成交叉编译配置..."
cat <<EOF > mingw-toolchain.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# 3. 编译 Linux 版本
echo "[3/4] 开始编译 Linux 动态库 (.so)..."
rm -rf build_linux
mkdir -p build_linux && cd build_linux
cmake ..
make -j$(nproc)
cd ..

# 4. 编译 Windows 版本 (使用 MinGW 交叉编译验证)
echo "[4/4] 开始编译 Windows 动态库 (.dll)..."
rm -rf build_windows
mkdir -p build_windows && cd build_windows
cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake ..
make -j$(nproc)
cd ..

echo "=================================================="
echo "                    构建成功!                     "
echo "=================================================="
echo "📁 Linux 产物路径   : build_linux/"
echo "   - 动态库 : build_linux/libCodroid.so"
echo "   - 测试程序: build_linux/CodroidExample"
echo "   ▶ 运行测试: cd build_linux && export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH && ./CodroidExample"
echo ""
echo "📁 Windows 产物路径 : build_windows/"
echo "   - 动态库 : build_windows/libCodroid.dll"
echo "   - 导入库 : build_windows/libCodroid.dll.a"
echo ""
echo "🚀 源码迁移指南 (针对 Windows MSVC):"
echo "1. 直接将整个 CodroidSDK 文件夹拷贝到 Windows。"
echo "2. third_party 文件夹内已包含所有头文件，无需额外下载。"
echo "3. 在 Windows 上打开 CMake GUI 或 VS Code，选择 MSVC 编译器进行编译。"
echo "=================================================="