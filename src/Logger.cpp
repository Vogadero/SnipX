// 日志系统实现

#include "Logger.h"
#include <shlobj.h>
#include <ctime>
#include <cstdarg>

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_minLevel(LOG_INFO)
    , m_initialized(false)
{
    InitializeCriticalSection(&m_cs);
}

Logger::~Logger()
{
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
    DeleteCriticalSection(&m_cs);
}

void Logger::Initialize(const std::wstring& logPath)
{
    EnterCriticalSection(&m_cs);
    
    if (!m_initialized)
    {
        // 如果没有指定路径，使用默认路径
        std::wstring path = logPath;
        if (path.empty())
        {
            WCHAR appDataPath[MAX_PATH];
            if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
            {
                path = appDataPath;
                path += L"\\SnipX\\snipx.log";
                
                // 确保目录存在
                std::wstring dir = appDataPath;
                dir += L"\\SnipX";
                CreateDirectoryW(dir.c_str(), NULL);
            }
        }
        
        m_logFile.open(path.c_str(), std::ios::out | std::ios::app);
        if (m_logFile.is_open())
        {
            m_initialized = true;
            m_logFile << L"\n========== SnipX Started at " << GetTimestamp() << L" ==========\n";
            m_logFile.flush();
        }
    }
    
    LeaveCriticalSection(&m_cs);
}

void Logger::SetLevel(LogLevel level)
{
    m_minLevel = level;
}

void Logger::Log(LogLevel level, const wchar_t* format, ...)
{
    if (!m_initialized || level < m_minLevel)
        return;
    
    EnterCriticalSection(&m_cs);
    
    if (m_logFile.is_open())
    {
        // 格式化消息
        wchar_t buffer[4096];
        va_list args;
        va_start(args, format);
        vswprintf_s(buffer, 4096, format, args);
        va_end(args);
        
        // 写入日志
        m_logFile << L"[" << GetTimestamp() << L"] "
                  << L"[" << GetLevelString(level) << L"] "
                  << buffer << L"\n";
        m_logFile.flush();
    }
    
    LeaveCriticalSection(&m_cs);
}

void Logger::Flush()
{
    EnterCriticalSection(&m_cs);
    if (m_logFile.is_open())
    {
        m_logFile.flush();
    }
    LeaveCriticalSection(&m_cs);
}

void Logger::Debug(const wchar_t* format, ...)
{
    if (!m_initialized || LOG_DEBUG < m_minLevel)
        return;
    
    wchar_t buffer[4096];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, 4096, format, args);
    va_end(args);
    
    Log(LOG_DEBUG, L"%s", buffer);
}

void Logger::Info(const wchar_t* format, ...)
{
    if (!m_initialized || LOG_INFO < m_minLevel)
        return;
    
    wchar_t buffer[4096];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, 4096, format, args);
    va_end(args);
    
    Log(LOG_INFO, L"%s", buffer);
}

void Logger::Warning(const wchar_t* format, ...)
{
    if (!m_initialized || LOG_WARNING < m_minLevel)
        return;
    
    wchar_t buffer[4096];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, 4096, format, args);
    va_end(args);
    
    Log(LOG_WARNING, L"%s", buffer);
}

void Logger::Error(const wchar_t* format, ...)
{
    if (!m_initialized || LOG_ERROR < m_minLevel)
        return;
    
    wchar_t buffer[4096];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, 4096, format, args);
    va_end(args);
    
    Log(LOG_ERROR, L"%s", buffer);
}

std::wstring Logger::GetTimestamp()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    wchar_t buffer[64];
    swprintf_s(buffer, L"%04d-%02d-%02d %02d:%02d:%02d.%03d",
              st.wYear, st.wMonth, st.wDay,
              st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    
    return buffer;
}

const wchar_t* Logger::GetLevelString(LogLevel level)
{
    switch (level)
    {
    case LOG_DEBUG:   return L"DEBUG";
    case LOG_INFO:    return L"INFO ";
    case LOG_WARNING: return L"WARN ";
    case LOG_ERROR:   return L"ERROR";
    default:          return L"UNKN ";
    }
}
