// 日志系统

#ifndef LOGGER_H
#define LOGGER_H

#include <windows.h>
#include <string>
#include <fstream>

/**
 * 日志级别，数值越大表示越严重。
 */
enum LogLevel
{
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3
};

/**
 * 线程安全的简易文件日志单例。
 *
 * 低于当前阈值的日志会被丢弃；日志写入使用临界区保护。
 */
class Logger
{
public:
    /**
     * 获取全局日志单例。
     */
    static Logger& GetInstance();

    /**
     * 初始化日志文件。
     *
     * @param logPath 日志文件路径；空字符串时使用默认路径。
     */
    void Initialize(const std::wstring& logPath);

    /**
     * 设置最小输出级别。
     *
     * @param level 低于该级别的日志不会写出。
     */
    void SetLevel(LogLevel level);

    /**
     * 按级别写入格式化日志。
     *
     * @param level 日志级别。
     * @param format printf 风格宽字符串格式。
     */
    void Log(LogLevel level, const wchar_t* format, ...);

    /**
     * 立即刷新日志文件缓冲。
     */
    void Flush();

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
