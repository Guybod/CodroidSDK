@echo off
setlocal enabledelayedexpansion

:: 设置构建目录名称
set BUILD_DIR=build_msvc

echo ==================================================
echo      Codroid SDK MSVC Auto Builder (x64)
echo ==================================================

:: 1. 清理旧的构建目录
if exist %BUILD_DIR% (
    echo [1/4] Cleaning old build directory...
    rd /s /q %BUILD_DIR%
)

:: 2. 创建并进入目录
echo [2/4] Creating build directory...
mkdir %BUILD_DIR%
cd %BUILD_DIR%

:: 3. 配置 CMake (生成解决方案)
:: 注意：如果你使用的是 VS2019，请把 "Visual Studio 17 2022" 改为 "Visual Studio 16 2019"
echo [3/4] Configuring CMake for Visual Studio 2022...
cmake .. -G "Visual Studio 18 2026" -A x64
if %errorlevel% neq 0 goto ERROR

:: 4. 执行编译 - DEBUG 版本
echo [4/4] Building DEBUG configuration...
cmake --build . --config Debug
if %errorlevel% neq 0 goto ERROR

:: 5. 执行编译 - RELEASE 版本
echo Building RELEASE configuration...
cmake --build . --config Release
if %errorlevel% neq 0 goto ERROR

echo.
echo ==================================================
echo                BUILD SUCCESSFUL ^!
echo ==================================================
echo.
echo DEBUG FILES:
echo   DLL: %BUILD_DIR%\Debug\Codroid.dll
echo   LIB: %BUILD_DIR%\Debug\Codroid.lib
echo.
echo RELEASE FILES:
echo   DLL: %BUILD_DIR%\Release\Codroid.dll
echo   LIB: %BUILD_DIR%\Release\Codroid.lib
echo.
echo ==================================================
pause
exit /b 0

:ERROR
echo.
echo !!!!!!!!!!!!!!! BUILD FAILED !!!!!!!!!!!!!!!
echo.
pause
exit /b %errorlevel%
