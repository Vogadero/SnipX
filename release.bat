@echo off
REM SnipX 发布打包脚本
REM 用于创建发布版本和打包

echo ========================================
echo SnipX 发布打包脚本
echo ========================================
echo.

REM 设置版本号
set VERSION=1.1.0
set BUILD_DATE=%date:~0,10%

echo 版本号: %VERSION%
echo 构建日期: %BUILD_DATE%
echo.

REM 创建发布目录
if not exist "release" mkdir release
if not exist "release\SnipX-%VERSION%" mkdir "release\SnipX-%VERSION%"

echo [1/6] 清理旧文件...
del /Q "release\SnipX-%VERSION%\*.*" 2>nul

echo [2/6] 编译发布版本...
call build.bat
if %ERRORLEVEL% NEQ 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo [3/6] 复制文件...
copy "bin\SnipX.exe" "release\SnipX-%VERSION%\SnipX.exe"
copy "README.md" "release\SnipX-%VERSION%\README.md"
copy "USER_MANUAL.md" "release\SnipX-%VERSION%\用户手册.md"
copy "CHANGELOG.md" "release\SnipX-%VERSION%\更新日志.md"

echo [4/6] 创建版本信息文件...
echo SnipX v%VERSION% > "release\SnipX-%VERSION%\VERSION.txt"
echo 构建日期: %BUILD_DATE% >> "release\SnipX-%VERSION%\VERSION.txt"
echo. >> "release\SnipX-%VERSION%\VERSION.txt"
echo 极致轻量的 Windows 截图工具 >> "release\SnipX-%VERSION%\VERSION.txt"
echo. >> "release\SnipX-%VERSION%\VERSION.txt"
echo 功能特性: >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 区域截图、全屏截图、窗口截图 >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 10 种标注工具（矩形、椭圆、箭头、铅笔、文字、直线、序号、高亮、马赛克、模糊） >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 取色器（RGB/HSL/HEX） >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 贴图功能 >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 多格式保存（PNG/JPG/BMP） >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 屏幕录制和 GIF 导出 >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 多语言支持 >> "release\SnipX-%VERSION%\VERSION.txt"
echo. >> "release\SnipX-%VERSION%\VERSION.txt"
echo 系统要求: >> "release\SnipX-%VERSION%\VERSION.txt"
echo - Windows 7 或更高版本 >> "release\SnipX-%VERSION%\VERSION.txt"
echo - 无需安装，解压即用 >> "release\SnipX-%VERSION%\VERSION.txt"

echo [5/6] 检查文件大小...
for %%F in ("release\SnipX-%VERSION%\SnipX.exe") do (
    set SIZE=%%~zF
    set /a SIZE_KB=%%~zF/1024
    set /a SIZE_MB=%%~zF/1024/1024
)
echo SnipX.exe 大小: %SIZE_KB% KB

if %SIZE_KB% GTR 1024 (
    echo 警告：文件大小超过 1MB！
    echo 建议使用 UPX 压缩。
    echo.
    
    REM 检查是否有 UPX
    where upx >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo 检测到 UPX，是否压缩？[Y/N]
        set /p COMPRESS=
        if /i "%COMPRESS%"=="Y" (
            echo 正在压缩...
            upx --best --lzma "release\SnipX-%VERSION%\SnipX.exe"
            echo 压缩完成！
            for %%F in ("release\SnipX-%VERSION%\SnipX.exe") do (
                set /a COMPRESSED_KB=%%~zF/1024
            )
            echo 压缩后大小: !COMPRESSED_KB! KB
        )
    ) else (
        echo UPX 未安装，跳过压缩。
        echo 下载地址: https://upx.github.io/
    )
)

echo [6/6] 创建 ZIP 压缩包...
where 7z >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    7z a -tzip "release\SnipX-%VERSION%.zip" "release\SnipX-%VERSION%\*"
    echo ZIP 压缩包已创建: release\SnipX-%VERSION%.zip
) else (
    echo 7-Zip 未安装，跳过 ZIP 打包。
    echo 请手动压缩 release\SnipX-%VERSION% 目录。
)

echo.
echo ========================================
echo 发布打包完成！
echo ========================================
echo.
echo 发布目录: release\SnipX-%VERSION%
echo.
echo 下一步:
echo 1. 测试 release\SnipX-%VERSION%\SnipX.exe
echo 2. 上传到 GitHub Release
echo 3. 更新官网下载链接
echo.

pause
