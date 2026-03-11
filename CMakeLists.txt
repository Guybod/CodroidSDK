cmake_minimum_required(VERSION 3.10)
project(CodroidSDK LANGUAGES CXX)

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601) # 指定支持 Windows 7 及以上系统
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 核心宏：所有平台都需要
add_definitions(-DASIO_STANDALONE)

# 2. 针对 Windows MSVC 的特殊处理
if(MSVC)
    # Asio 在 Windows 下需要定义目标版本，0x0601 代表 Windows 7
    add_definitions(-D_WIN32_WINNT=0x0601)
    # 禁用 MSVC 对一些标准函数的安全警告
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # 强制 MSVC 使用 UTF-8 处理源码 (防止中文注释报错)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

# 3. 包含路径 (使用我们物理复制后的 third_party)
include_directories(include)
include_directories(SYSTEM third_party)

# 4. 定义源文件
set(LIB_SOURCES src/CodroidControlInterface.cpp
                src/CodroidSubscriber.cpp
)

# 5. 构建动态库
add_library(Codroid SHARED ${LIB_SOURCES})
target_compile_definitions(Codroid PRIVATE CODROID_EXPORTS)

# 6. 针对不同操作系统的链接处理
if(WIN32)
    # 无论是 MSVC 还是 MinGW，在 Windows 下都需要网络库
    target_link_libraries(Codroid PRIVATE ws2_32 mswsock)
    
    # 只有在使用 MinGW 编译时才添加静态链接选项
    if(MINGW)
        target_link_options(Codroid PRIVATE "-static-libgcc" "-static-libstdc++")
    endif()
    
    # 设置 DLL 导出名称
    set_target_properties(Codroid PROPERTIES OUTPUT_NAME "Codroid")
else()
    # Linux 配置
    set_target_properties(Codroid PROPERTIES OUTPUT_NAME "Codroid")
    target_link_libraries(Codroid PRIVATE pthread)
endif()

# 7. 示例程序 (仅在本地开发/测试时构建)
if(NOT CMAKE_CROSSCOMPILING)
    # 获取 examples 目录下所有的 .cpp 文件
    file(GLOB EXAMPLE_SOURCES "examples/*.cpp")

    foreach(test_file ${EXAMPLE_SOURCES})
        # 提取文件名（不含路径和后缀），比如 jog_test
        get_filename_component(test_name ${test_file} NAME_WE)
        
        # 为每个文件创建一个可执行程序
        add_executable(${test_name} ${test_file})
        
        # 链接动态库
        target_link_libraries(${test_name} PRIVATE Codroid)
        
        # 输出一下进度，方便查看生成了哪些测试程序
        message(STATUS "Adding Example: ${test_name}")
    endforeach()
    
endif()