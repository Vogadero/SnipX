@echo off
REM SnipX 优化检查和执行脚本

echo ========================================
echo SnipX 优化检查脚本
echo ========================================
echo.

REM 检查是否已编译
if not exist "bin\SnipX.exe" (
    echo 错误：未找到 bin\SnipX.exe
    echo 请先运行 build.bat 编译项目
    pause
    exit /b 1
)

echo [1/5] 检查文件大小...
for %%F in ("bin\SnipX.exe") do (
    set SIZE=%%~zF
    set /a SIZE_KB=%%~zF/1024
    set /a SIZE_MB=%%~zF/1024/1024
)

echo 当前大小: %SIZE_KB% KB

if %SIZE_KB% GTR 1024 (
    echo ⚠️  警告：文件大小超过 1MB 目标
    echo.
    echo 建议优化措施：
    echo 1. 使用 /O2 /GL 编译优化
    echo 2. 使用 /LTCG 链接优化
    echo 3. 使用 UPX 压缩
    echo.
) else (
    echo ✅ 文件大小符合目标（< 1MB）
    echo.
)

echo [2/5] 检查依赖库...
dumpbin /dependents bin\SnipX.exe > temp_deps.txt 2>nul
if %ERRORLEVEL% EQU 0 (
    echo 依赖库列表：
    findstr /C:".dll" temp_deps.txt
    del temp_deps.txt
    echo.
) else (
    echo 注意：dumpbin 不可用，跳过依赖检查
    echo 请安装 Visual Studio 或 Windows SDK
    echo.
)

echo [3/5] 检查 UPX 压缩...
where upx >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo UPX 已安装
    echo.
    echo 是否使用 UPX 压缩？[Y/N]
    set /p COMPRESS=
    if /i "%COMPRESS%"=="Y" (
        echo 正在压缩...
        copy bin\SnipX.exe bin\SnipX_original.exe
        upx --best --lzma bin\SnipX.exe
        
        for %%F in ("bin\SnipX.exe") do (
            set /a COMPRESSED_KB=%%~zF/1024
        )
        
        echo.
        echo 压缩完成！
        echo 原始大小: %SIZE_KB% KB
        echo 压缩后大小: %COMPRESSED_KB% KB
        set /a SAVED_KB=%SIZE_KB%-%COMPRESSED_KB%
        set /a RATIO=%SAVED_KB%*100/%SIZE_KB%
        echo 节省空间: %SAVED_KB% KB (%RATIO%%%)
        echo.
        echo 原始文件已备份为: bin\SnipX_original.exe
        echo.
    )
) else (
    echo ⚠️  UPX 未安装
    echo 下载地址: https://upx.github.io/
    echo.
)

echo [4/5] 代码质量检查...
echo.
echo 检查项目：
echo - [ ] 所有公共 API 都有注释
echo - [ ] 没有内存泄漏
echo - [ ] 没有未使用的变量
echo - [ ] 没有警告
echo - [ ] 代码符合规范
echo.
echo 请手动检查以上项目
echo.

echo [5/5] 性能测试建议...
echo.
echo 建议进行以下测试：
echo 1. 启动时间测试（目标 < 500ms）
echo 2. 内存占用测试（目标 < 10MB 空闲）
echo 3. 截图速度测试（目标 < 100ms）
echo 4. CPU 占用测试（目标 < 1%% 空闲）
echo.

echo ========================================
echo 优化检查完成
echo ========================================
echo.

echo 下一步建议：
echo 1. 运行 bin\SnipX.exe 测试功能
echo 2. 检查日志文件 %%APPDATA%%\SnipX\snipx.log
echo 3. 进行兼容性测试
echo 4. 准备发布
echo.

pause
