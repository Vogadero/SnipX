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
    /**
     * 尝试启用 Per-Monitor V2 DPI 感知。
     *
     * 旧系统若缺少 API 会静默跳过，避免影响启动。
     */
    void EnableDpiAwareness()
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32)
            return;

        // 动态解析，兼容未导出该 API 的旧 Windows 版本
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

/**
 * Windows GUI 程序入口。
 *
 * 初始化 DPI 感知、日志、COM 与 SnipXApp，然后进入主消息循环。
 */
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

    // 初始化 COM，供部分 Shell/对话框接口使用
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
            g_pApp = nullptr;
            CoUninitialize();
            return -1;
        }
    }

    LOG_INFO(L"SnipXApp initialized successfully");
    PERF_LOG_MEMORY();

    // 标准 Win32 消息循环；Exit() 会通过 PostQuitMessage 结束
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
    g_pApp = nullptr;
    CoUninitialize();

    // 检查内存泄漏（仅 Debug 模式）
    MEMORY_CHECK_LEAKS();

    return (int)msg.wParam;
}
