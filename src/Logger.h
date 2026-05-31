// 日志系统

#ifndef LOGGER_H
#define LOGGER_H

#include <windows.h>
#include <string>
#include <fstream>

enum LogLevel
{
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3
};

class Logger
{
public:
    static Logger& GetInstance();
    
    void Initialize(const std::wstring& logPath);
    void SetLevel(LogLevel level);
    void Log(LogLevel level, const wchar_t* format, ...);
    void Flush();
    
    // 便捷宏
    void Debug(const wchar_t* format, ...);
    void Info(const wchar_t* format, ...);
    void Warning(const wchar_t* format, ...);
    void Error(const wchar_t* format, ...);
    
private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::wstring GetTimestamp();
    const wchar_t* GetLevelString(LogLevel level);
    
    std::wofstream m_logFile;
    LogLevel m_minLevel;
    CRITICAL_SECTION m_cs;
    bool m_initialized;
};

// 全局日志宏
#define LOG_DEBUG(...) Logger::GetInstance().Debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::GetInstance().Info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::GetInstance().Warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::GetInstance().Error(__VA_ARGS__)

#endif // LOGGER_H
