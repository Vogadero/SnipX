// 性能监控工具
// 用于测量和分析性能

#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <windows.h>
#include <psapi.h>
#include <string>
#include "Logger.h"

#pragma comment(lib, "psapi.lib")

/**
 * 高精度性能计时器，基于 QueryPerformanceCounter。
 */
class PerformanceTimer
{
private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startTime;
    const wchar_t* m_name;
    bool m_running;

public:
    PerformanceTimer(const wchar_t* name = L"Timer")
        : m_name(name)
        , m_running(false)
    {
        QueryPerformanceFrequency(&m_frequency);
    }

    /**
     * 开始计时。
     */
    void Start()
    {
        QueryPerformanceCounter(&m_startTime);
        m_running = true;
    }

    /**
     * 停止计时并返回经过的时间（秒）。
     *
     * @return 未启动时返回 0。
     */
    double Stop()
    {
        if (!m_running)
            return 0.0;

        LARGE_INTEGER endTime;
        QueryPerformanceCounter(&endTime);

        double elapsed = (double)(endTime.QuadPart - m_startTime.QuadPart) / m_frequency.QuadPart;
        m_running = false;

        return elapsed;
    }

    /**
     * 停止计时并写入日志。
     *
     * @return 经过时间（秒）。
     */
    double StopAndLog()
    {
        double elapsed = Stop();
        LOG_INFO(L"%s took %.3f ms", m_name, elapsed * 1000.0);
        return elapsed;
    }

    /**
     * 获取当前经过的时间（不停止计时）。
     *
     * @return 经过时间（秒）。
     */
    double Elapsed() const
    {
        if (!m_running)
            return 0.0;

        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        return (double)(currentTime.QuadPart - m_startTime.QuadPart) / m_frequency.QuadPart;
    }
};

/**
 * 作用域计时器：构造时启动，析构时自动记录耗时。
 */
class ScopedTimer
{
private:
    PerformanceTimer m_timer;

public:
    ScopedTimer(const wchar_t* name)
        : m_timer(name)
    {
        m_timer.Start();
    }

    ~ScopedTimer()
    {
        m_timer.StopAndLog();
    }
};

/**
 * 进程内存/CPU 等运行时指标采集工具。
 */
class PerformanceMonitor
{
public:
    /**
     * 获取当前进程工作集与私有字节数。
     *
     * @param workingSet 输出工作集字节数。
     * @param privateUsage 输出私有字节数。
     * @return 成功时返回 true。
     */
    static bool GetMemoryUsage(SIZE_T& workingSet, SIZE_T& privateUsage)
    {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(),
                                 (PROCESS_MEMORY_COUNTERS*)&pmc,
                                 sizeof(pmc)))
        {
            workingSet = pmc.WorkingSetSize;
            privateUsage = pmc.PrivateUsage;
            return true;
        }
        return false;
    }

    /**
     * 获取内存使用情况（MB）。
     */
    static bool GetMemoryUsageMB(double& workingSetMB, double& privateUsageMB)
    {
        SIZE_T workingSet, privateUsage;
        if (GetMemoryUsage(workingSet, privateUsage))
        {
            workingSetMB = workingSet / (1024.0 * 1024.0);
            privateUsageMB = privateUsage / (1024.0 * 1024.0);
            return true;
        }
        return false;
    }

    /**
     * 将当前内存使用写入日志。
     */
    static void LogMemoryUsage()
    {
        double workingSetMB, privateUsageMB;
        if (GetMemoryUsageMB(workingSetMB, privateUsageMB))
        {
            LOG_INFO(L"Memory Usage: Working Set = %.2f MB, Private = %.2f MB",
                     workingSetMB, privateUsageMB);
        }
    }

    /**
     * 估算当前进程 CPU 使用率（需多次调用形成采样差）。
     *
     * @return 大致百分比；采样不足时可能为 0。
     */
    static double GetCPUUsage()
    {
        static ULONGLONG lastCPU = 0;
        static ULONGLONG lastSysCPU = 0;
        static ULONGLONG lastUserCPU = 0;
        static int numProcessors = 0;
        
        if (numProcessors == 0)
        {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            numProcessors = sysInfo.dwNumberOfProcessors;
        }
        
        FILETIME ftime, fsys, fuser;
        ULONGLONG now, sys, user;
        
        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));
        
        HANDLE hProcess = GetCurrentProcess();
        GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        
        double percent = 0.0;
        
        if (lastCPU != 0)
        {
            percent = (sys - lastSysCPU) + (user - lastUserCPU);
            percent /= (now - lastCPU);
            percent /= numProcessors;
            percent *= 100.0;
        }
        
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;
        
        return percent;
    }
    
    // 记录 CPU 使用率
    static void LogCPUUsage()
    {
        double cpuUsage = GetCPUUsage();
        LOG_INFO(L"CPU Usage: %.2f%%", cpuUsage);
    }
    
    // 获取 GDI 对象数量
    static DWORD GetGDIObjectCount()
    {
        return GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    }
    
    // 获取 USER 对象数量
    static DWORD GetUSERObjectCount()
    {
        return GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);
    }
    
    // 记录 GDI/USER 对象数量
    static void LogGDIUsage()
    {
        DWORD gdiCount = GetGDIObjectCount();
        DWORD userCount = GetUSERObjectCount();
        LOG_INFO(L"GDI Objects: %d, USER Objects: %d", gdiCount, userCount);
    }
    
    // 完整的性能报告
    static void LogPerformanceReport()
    {
        LOG_INFO(L"=== Performance Report ===");
        LogMemoryUsage();
        LogCPUUsage();
        LogGDIUsage();
        LOG_INFO(L"=========================");
    }
};

// 性能监控宏
#ifdef _DEBUG
#define PERF_TIMER_START(name) PerformanceTimer __perfTimer##__LINE__(name); __perfTimer##__LINE__.Start()
#define PERF_TIMER_STOP() __perfTimer##__LINE__.StopAndLog()
#define PERF_SCOPE(name) ScopedTimer __scopedTimer##__LINE__(name)
#define PERF_LOG_MEMORY() PerformanceMonitor::LogMemoryUsage()
#define PERF_LOG_CPU() PerformanceMonitor::LogCPUUsage()
#define PERF_LOG_GDI() PerformanceMonitor::LogGDIUsage()
#define PERF_LOG_REPORT() PerformanceMonitor::LogPerformanceReport()
#else
#define PERF_TIMER_START(name)
#define PERF_TIMER_STOP()
#define PERF_SCOPE(name)
#define PERF_LOG_MEMORY()
#define PERF_LOG_CPU()
#define PERF_LOG_GDI()
#define PERF_LOG_REPORT()
#endif

#endif // PERFORMANCE_MONITOR_H
