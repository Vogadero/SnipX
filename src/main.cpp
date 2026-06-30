// SnipX - 极致轻量截图工具
// 主程序入口

#include "SnipX.h"
#include "Platform.h"
#include "Logger.h"
#include "MemoryChecker.h"
#include "PerformanceMonitor.h"
#include "Version.h"
#include <windows.h>
#include <shellapi.h>

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

namespace
{
    void EnableDpiAwareness()
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32)
            return;

        typedef BOOL(WINAPI* SetProcessDpiAwarenessContextProc)(DPI_AWARENESS_CONTEXT);
        auto setDpiAwarenessContext = (SetProcessDpiAwarenessContextProc)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
        if (setDpiAwarenessContext)
        {
            setDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }
}

// 全局变量
HINSTANCE g_hInstance = NULL;
SnipXApp* g_pApp = NULL;

// WinMain入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    EnableDpiAwareness();

    // 初始化内存检查（仅 Debug 模式）
    MEMORY_CHECK_INIT();
    
    // 初始化日志系统
    Logger::GetInstance().Initialize(L"");
    LOG_INFO(L"SnipX v%s starting...", GetVersionString());
    
    // 记录初始性能状态
    PERF_LOG_MEMORY();
    
    // 初始化COM
    CoInitialize(NULL);
    LOG_INFO(L"COM initialized");
    
    // 创建应用程序实例
    {
        PERF_SCOPE(L"Application Initialization");
        g_pApp = new SnipXApp(hInstance);
        
        if (!g_pApp->Initialize())
        {
            LOG_ERROR(L"Failed to initialize SnipXApp");
            delete g_pApp;
            CoUninitialize();
            return -1;
        }
    }
    
    LOG_INFO(L"SnipXApp initialized successfully");
    PERF_LOG_MEMORY();
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    LOG_INFO(L"SnipX shutting down...");
    
    // 记录最终性能状态
    PERF_LOG_REPORT();
    
    // 清理
    delete g_pApp;
    CoUninitialize();
    
    // 检查内存泄漏（仅 Debug 模式）
    MEMORY_CHECK_LEAKS();
    
    return (int)msg.wParam;
}
