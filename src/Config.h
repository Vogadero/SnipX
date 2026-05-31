// 配置管理类

#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include <string>
#include <vector>


// 启动模式
enum StartupMode
{
    STARTUP_TRAY = 0,      // 启动后驻留托盘
    STARTUP_CAPTURE = 1    // 启动即截图
};

struct HistoryItem
{
    std::wstring path;
    std::wstring timestamp;
};

// 退出模式

enum ExitMode
{
    EXIT_TRAY = 0,         // 截图后驻留托盘
    EXIT_QUIT = 1          // 截图后退出
};

class Config
{
public:
    Config();
    ~Config();
    
    bool Load();
    bool Save();
    
    // 热键配置
    UINT GetHotkey() const { return m_hotkey; }
    void SetHotkey(UINT hotkey) { m_hotkey = hotkey; }
    UINT GetFullScreenHotkey() const { return m_fullScreenHotkey; }
    void SetFullScreenHotkey(UINT hotkey) { m_fullScreenHotkey = hotkey; }

    // 上次选区配置
    bool HasLastSelection() const { return m_hasLastSelection; }
    RECT GetLastSelection() const { return m_lastSelection; }
    void SetLastSelection(const RECT& rect);
    void ClearLastSelection();
    
    // 历史记录配置
    std::vector<HistoryItem> GetHistoryItems() const { return m_historyItems; }
    void AddHistoryItem(const std::wstring& path);
    void ClearHistoryItems();

    // 保存配置

    std::wstring GetDefaultFormat() const { return m_defaultFormat; }

    std::wstring GetDefaultPath() const { return m_defaultPath; }
    int GetJpgQuality() const { return m_jpgQuality; }
    bool IsAutoSave() const { return m_autoSave; }
    std::wstring GetAutoSavePath() const { return m_autoSavePath; }
    std::wstring GetEffectiveAutoSavePath() const;
    std::wstring GetFileNamePrefix() const { return m_fileNamePrefix; }

    
    void SetDefaultFormat(const std::wstring& format) { m_defaultFormat = format; }
    void SetDefaultPath(const std::wstring& path) { m_defaultPath = path; }
    void SetJpgQuality(int quality) { m_jpgQuality = quality; }
    void SetAutoSave(bool autoSave) { m_autoSave = autoSave; }
    void SetAutoSavePath(const std::wstring& path) { m_autoSavePath = path; }
    void SetFileNamePrefix(const std::wstring& prefix) { m_fileNamePrefix = prefix; }
    
    // 外观配置
    bool IsTrayIconVisible() const { return m_trayIconVisible; }
    StartupMode GetStartupMode() const { return m_startupMode; }
    ExitMode GetExitMode() const { return m_exitMode; }
    void SetTrayIconVisible(bool visible) { m_trayIconVisible = visible; }
    void SetStartupMode(StartupMode mode) { m_startupMode = mode; }
    void SetExitMode(ExitMode mode) { m_exitMode = mode; }
    
    // 标注默认值
    COLORREF GetDefaultColor() const { return m_defaultColor; }
    int GetDefaultStrokeWidth() const { return m_defaultStrokeWidth; }
    void SetDefaultColor(COLORREF color) { m_defaultColor = color; }
    void SetDefaultStrokeWidth(int width) { m_defaultStrokeWidth = width; }
    
    // 开机自启
    bool IsAutoStart() const { return m_autoStart; }
    void SetAutoStart(bool autoStart);
    
private:
    std::wstring GetConfigPath();
    void SetDefaultValues();
    
    // 热键
    UINT m_hotkey;  // VK_X + MOD_ALT
    UINT m_fullScreenHotkey;  // VK_F + MOD_ALT

    // 上次选区
    bool m_hasLastSelection;
    RECT m_lastSelection;

    // 历史记录
    std::vector<HistoryItem> m_historyItems;
    int m_historyLimit;
    
    // 保存


    std::wstring m_defaultFormat;
    std::wstring m_defaultPath;
    int m_jpgQuality;
    bool m_autoSave;
    std::wstring m_autoSavePath;
    std::wstring m_fileNamePrefix;
    
    // 外观
    bool m_trayIconVisible;
    StartupMode m_startupMode;
    ExitMode m_exitMode;
    
    // 标注
    COLORREF m_defaultColor;
    int m_defaultStrokeWidth;
    
    // 启动
    bool m_autoStart;
};

#endif // CONFIG_H
