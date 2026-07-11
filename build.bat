@echo off
REM SnipX 编译脚本 - 使用 MinGW-w64 或 MSVC

echo ========================================
echo SnipX 极致轻量截图工具 - 编译脚本
echo ========================================
echo.

REM 检查编译器
where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo 检测到 MSVC 编译器
    goto :BUILD_MSVC
)

where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo 检测到 MinGW 编译器
    goto :BUILD_MINGW
)

echo 错误：未找到编译器！
echo 请安装 Visual Studio 或 MinGW-w64
if not defined CI pause
exit /b 1

:BUILD_MSVC
echo.
echo 使用 MSVC 编译...
echo.

REM 创建输出目录
if not exist "bin" mkdir bin
if not exist "obj" mkdir obj

REM 编译资源文件
rc /fo obj\SnipX.res res\SnipX.rc

REM 编译源文件
cl /c /O2 /GL /EHsc /MT /utf-8 /W3 /nologo ^
   /I"src" ^
   /Fo"obj\\" ^
   src\main.cpp ^
   src\SnipX.cpp ^
   src\Config.cpp ^
   src\HotkeyManager.cpp ^
   src\TrayIcon.cpp ^
   src\ScreenCapture.cpp ^
   src\ScreenRecorder.cpp ^
   src\Localization.cpp ^
   src\Platform.cpp ^
   src\GdiplusUtils.cpp ^
   src\Logger.cpp ^
   src\Editor.cpp ^
   src\ColorPicker.cpp ^
   src\PinWindow.cpp ^
   src\SettingsDialog.cpp

if %ERRORLEVEL% NEQ 0 (
    echo 编译失败！
    if not defined CI pause
    exit /b 1
)

REM 链接
link /OUT:bin\SnipX.exe /SUBSYSTEM:WINDOWS /LTCG /OPT:REF /OPT:ICF ^
     /NOLOGO ^
     obj\main.obj ^
     obj\SnipX.obj ^
     obj\Config.obj ^
     obj\HotkeyManager.obj ^
     obj\TrayIcon.obj ^
     obj\ScreenCapture.obj ^
     obj\ScreenRecorder.obj ^
     obj\Localization.obj ^
     obj\Platform.obj ^
     obj\GdiplusUtils.obj ^
     obj\Logger.obj ^
     obj\Editor.obj ^
     obj\ColorPicker.obj ^
     obj\PinWindow.obj ^
     obj\SettingsDialog.obj ^
     obj\SnipX.res ^
     gdiplus.lib shell32.lib user32.lib gdi32.lib ole32.lib dwmapi.lib shlwapi.lib advapi32.lib comctl32.lib comdlg32.lib wininet.lib psapi.lib

if %ERRORLEVEL% NEQ 0 (
    echo 链接失败！
    if not defined CI pause
    exit /b 1
)

echo.
echo 编译成功！
dir bin\SnipX.exe
if not defined CI pause
exit /b 0

:BUILD_MINGW
echo.
echo 使用 MinGW 编译...
echo.

REM 创建输出目录
if not exist "bin" mkdir bin
if not exist "obj" mkdir obj

REM 编译资源文件
windres res\SnipX.rc -O coff -o obj\SnipX.res

REM 编译并链接
g++ -o bin\SnipX.exe ^
    -O3 -s -static -mwindows ^
    -I"src" ^
    src\main.cpp ^
    src\SnipX.cpp ^
    src\Config.cpp ^
    src\HotkeyManager.cpp ^
    src\TrayIcon.cpp ^
    src\ScreenCapture.cpp ^
    src\ScreenRecorder.cpp ^
    src\Localization.cpp ^
    src\Platform.cpp ^
    src\GdiplusUtils.cpp ^
    src\Logger.cpp ^
    src\Editor.cpp ^
    src\ColorPicker.cpp ^
    src\PinWindow.cpp ^
    src\SettingsDialog.cpp ^
    obj\SnipX.res ^
    -lgdiplus -lshell32 -luser32 -lgdi32 -lole32 -ldwmapi -lshlwapi -ladvapi32 -lcomctl32 -lcomdlg32 -lwininet -lpsapi


if %ERRORLEVEL% NEQ 0 (
    echo 编译失败！
    if not defined CI pause
    exit /b 1
)

echo.
echo 编译成功！
dir bin\SnipX.exe
if not defined CI pause
exit /b 0
