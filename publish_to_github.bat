@echo off
REM SnipX GitHub 发布脚本
REM 使用前请先设置环境变量 GITHUB_TOKEN

echo ========================================
echo SnipX GitHub 发布脚本
echo ========================================
echo.

REM 检查是否设置了 GITHUB_TOKEN
if "%GITHUB_TOKEN%"=="" (
    echo ========================================
    echo 错误：未设置 GITHUB_TOKEN 环境变量
    echo ========================================
    echo.
    echo 请按以下步骤操作：
    echo.
    echo 1. 访问 https://github.com/settings/tokens
    echo 2. 点击 "Generate new token" ^(classic^)
    echo 3. 勾选 "repo" 权限
    echo 4. 生成并复制 token
    echo 5. 在命令行中设置环境变量：
    echo    set GITHUB_TOKEN=^<your_new_token^>
    echo 6. 重新运行此脚本
    echo.
    echo 注意：请勿在脚本中硬编码 token！
    echo.
    pause
    exit /b 1
)

REM 设置变量
set GITHUB_USER=vogadero
set REPO_NAME=SnipX
set VERSION=1.1.1
set TAG=v%VERSION%

echo GitHub 用户: %GITHUB_USER%
echo 仓库名称: %REPO_NAME%
echo 版本号: %VERSION%
echo 标签: %TAG%
echo.

REM 检查是否已初始化 Git
if not exist ".git" (
    echo [1/8] 初始化 Git 仓库...
    git init
    git add .
    git commit -m "Initial commit: SnipX v%VERSION%"
) else (
    echo [1/8] Git 仓库已存在
    echo 提交当前更改...
    git add .
    git commit -m "Release v%VERSION%"
)

echo.
echo [2/8] 检查远程仓库...
git remote -v | findstr origin >nul
if %ERRORLEVEL% NEQ 0 (
    echo 添加远程仓库...
    git remote add origin https://github.com/%GITHUB_USER%/%REPO_NAME%.git
) else (
    echo 远程仓库已存在
)

echo.
echo [3/8] 推送到 GitHub...
git branch -M main
git push -u origin main

echo.
echo [4/8] 创建标签...
git tag -a %TAG% -m "Release %VERSION%"
git push origin %TAG%

echo.
echo [5/8] 编译项目...
call build.bat
if %ERRORLEVEL% NEQ 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo [6/8] 创建发布包...
call release.bat

echo.
echo [7/8] 创建 GitHub Release...
echo.
echo 请访问以下链接手动创建 Release：
echo https://github.com/%GITHUB_USER%/%REPO_NAME%/releases/new?tag=%TAG%
echo.
echo 或使用 GitHub CLI (gh)：
echo gh release create %TAG% release\SnipX-%VERSION%.zip release\SnipX-%VERSION%\SnipX.exe --title "SnipX v%VERSION%" --notes-file RELEASE_NOTES.md
echo.

echo [8/8] 完成！
echo.
echo ========================================
echo 发布完成
echo ========================================
echo.
echo 下一步：
echo 1. 访问 https://github.com/%GITHUB_USER%/%REPO_NAME%/releases
echo 2. 创建新的 Release
echo 3. 上传发布文件
echo.

pause
