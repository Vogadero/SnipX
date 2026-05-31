// 内存泄漏检查工具
// 仅在 Debug 模式下启用

#ifndef MEMORY_CHECKER_H
#define MEMORY_CHECKER_H

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <windows.h>

// 内存泄漏检查器类
class MemoryChecker
{
public:
    // 初始化内存检查
    static void Initialize()
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        
        // 设置内存泄漏报告模式
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    }
    
    // 设置内存断点（在特定分配编号处中断）
    static void SetBreakAlloc(long allocNumber)
    {
        _CrtSetBreakAlloc(allocNumber);
    }
    
    // 检查内存泄漏
    static bool CheckMemoryLeaks()
    {
        return _CrtDumpMemoryLeaks() != 0;
    }
    
    // 获取当前内存状态
    static void GetMemoryState(_CrtMemState* state)
    {
        _CrtMemCheckpoint(state);
    }
    
    // 比较两个内存状态
    static bool CompareMemoryState(const _CrtMemState* oldState, 
                                   const _CrtMemState* newState,
                                   _CrtMemState* diffState)
    {
        return _CrtMemDifference(diffState, oldState, newState) != 0;
    }
    
    // 输出内存状态
    static void DumpMemoryState(const _CrtMemState* state)
    {
        _CrtMemDumpStatistics(state);
    }
    
    // 验证堆完整性
    static bool ValidateHeap()
    {
        return _CrtCheckMemory() != 0;
    }
    
    // 输出所有对象
    static void DumpAllObjects()
    {
        _CrtMemDumpAllObjectsSince(NULL);
    }
};

// 内存泄漏检查宏
#define MEMORY_CHECK_INIT() MemoryChecker::Initialize()
#define MEMORY_CHECK_LEAKS() MemoryChecker::CheckMemoryLeaks()
#define MEMORY_CHECK_HEAP() MemoryChecker::ValidateHeap()

// 内存状态快照宏
#define MEMORY_STATE_DECLARE(name) _CrtMemState name
#define MEMORY_STATE_CHECKPOINT(state) MemoryChecker::GetMemoryState(&state)
#define MEMORY_STATE_COMPARE(old, new, diff) MemoryChecker::CompareMemoryState(&old, &new, &diff)
#define MEMORY_STATE_DUMP(state) MemoryChecker::DumpMemoryState(&state)

// 作用域内存检查器
class ScopedMemoryChecker
{
private:
    _CrtMemState m_startState;
    const char* m_name;
    
public:
    ScopedMemoryChecker(const char* name) : m_name(name)
    {
        _CrtMemCheckpoint(&m_startState);
    }
    
    ~ScopedMemoryChecker()
    {
        _CrtMemState endState, diffState;
        _CrtMemCheckpoint(&endState);
        
        if (_CrtMemDifference(&diffState, &m_startState, &endState))
        {
            char buffer[256];
            sprintf_s(buffer, "Memory leak detected in scope: %s\n", m_name);
            OutputDebugStringA(buffer);
            _CrtMemDumpStatistics(&diffState);
        }
    }
};

// 作用域内存检查宏
#define MEMORY_SCOPE_CHECK(name) ScopedMemoryChecker __memChecker##__LINE__(name)

#else

// Release 模式下禁用所有宏
#define MEMORY_CHECK_INIT()
#define MEMORY_CHECK_LEAKS()
#define MEMORY_CHECK_HEAP()
#define MEMORY_STATE_DECLARE(name)
#define MEMORY_STATE_CHECKPOINT(state)
#define MEMORY_STATE_COMPARE(old, new, diff) false
#define MEMORY_STATE_DUMP(state)
#define MEMORY_SCOPE_CHECK(name)

#endif // _DEBUG

#endif // MEMORY_CHECKER_H
