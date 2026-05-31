@echo off
REM SnipX 网站部署脚本

echo ========================================
echo SnipX 网站部署脚本
echo ========================================
echo.

REM 检查 Node.js
node --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未安装 Node.js
    echo 请访问 https://nodejs.org/ 下载安装
    pause
    exit /b 1
)

echo [1/5] 检查依赖...
if not exist "node_modules" (
    echo 安装依赖...
    call npm install
    if %ERRORLEVEL% NEQ 0 (
        echo [错误] 依赖安装失败
        pause
        exit /b 1
    )
) else (
    echo 依赖已安装
)

echo.
echo [2/5] 构建网站...
call npm run docs:build
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 构建失败
    pause
    exit /b 1
)

echo.
echo [3/5] 检查构建输出...
if not exist ".vitepress\dist" (
    echo [错误] 构建输出不存在
    pause
    exit /b 1
)

echo.
echo [4/5] 预览网站...
echo.
echo 网站已构建完成！
echo.
echo 你可以：
echo 1. 运行 "npm run docs:preview" 预览网站
echo 2. 将 .vitepress\dist 目录部署到服务器
echo 3. 推送到 GitHub 自动部署到 GitHub Pages
echo.

echo [5/5] 完成！
echo.
echo ========================================
echo 部署选项
echo ========================================
echo.
echo 选择部署方式：
echo 1. 本地预览（推荐）
echo 2. 推送到 GitHub（自动部署）
echo 3. 退出
echo.

set /p choice="请选择 (1-3): "

if "%choice%"=="1" (
    echo.
    echo 启动预览服务器...
    call npm run docs:preview
) else if "%choice%"=="2" (
    echo.
    echo 推送到 GitHub...
    cd ..
    git add .
    git commit -m "Update documentation website"
    git push
    echo.
    echo 已推送到 GitHub！
    echo GitHub Actions 将自动构建和部署网站。
    echo.
    echo 查看部署状态：
    echo https://github.com/vogadero/SnipX/actions
    echo.
    echo 部署完成后访问：
    echo https://vogadero.github.io/SnipX/
    echo.
) else (
    echo 退出
)

pause
