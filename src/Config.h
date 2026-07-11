// 配置管理类

#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include <string>
#include <vector>

/**
 * 程序启动后的初始行为。
 */
enum StartupMode
{
    STARTUP_TRAY = 0,      // 启动后驻留托盘
    STARTUP_CAPTURE = 1    // 启动即截图
};

/**
 * 截图历史记录中的单项信息。
 */
struct HistoryItem
{
    std::wstring path;
    std::wstring timestamp;
};

/**
 * 截图/编辑完成后的退出行为。
 */
enum ExitMode
{
    EXIT_TRAY = 0,         // 截图后驻留托盘
    EXIT_QUIT = 1          // 截图后退出
};

/**
 * 负责读写应用配置、历史记录、上次选区以及开机自启注册表项。
 *
 * 配置默认持久化到 `%APPDATA%\\SnipX\\config.ini`。
 */
class Config
{
public:
    Config();
    ~Config();

    /**
     * 从配置文件加载设置；文件缺失时保留默认值。
     *
     * @return 配置路径可用时返回 true。
     */
    bool Load();

    /**
     * 将当前内存中的配置写回配置文件。
     *
     * @return 写入成功时返回 true。
     * @note 会同步写出热键、路径、历史记录和外观等全部字段。
     */
    bool Save();

    // 热键配置
    UINT GetHotkey() const { return m_hotkey; }
    void SetHotkey(UINT hotkey) { m_hotkey = hotkey; }
    UINT GetFullScreenHotkey() const { return m_fullScreenHotkey; }
    void SetFullScreenHotkey(UINT hotkey) { m_fullScreenHotkey = hotkey; }

    // 上次选区配置
    bool HasLastSelection() const { return m_hasLastSelection; }
    RECT GetLastSelection() const { return m_lastSelection; }

    /**
     * 保存最近一次有效截图选区。
     *
     * @param rect 虚拟桌面坐标中的选区；空矩形会被视为无上次选区。
     */
    void SetLastSelection(const RECT& rect);

    /**
     * 清空上次选区记忆。
     */
    void ClearLastSelection();

    // 历史记录配置
    /**
     * 返回当前历史记录副本。
     *
     * @return 按时间倒序排列的历史记录列表。
     */
    std::vector<HistoryItem> GetHistoryItems() const { return m_historyItems; }

    /**
     * 将保存路径插入历史记录头部；已存在路径会先去重再前置。
     *
     * @param path 截图文件完整路径。
     * @note 超过历史上限时会从尾部丢弃旧项。
     */
    void AddHistoryItem(const std::wstring& path);

    /**
     * 清空全部截图历史记录。
     */
    void ClearHistoryItems();

    // 保存配置
    std::wstring GetDefaultFormat() const { return m_defaultFormat; }
    std::wstring GetDefaultPath() const { return m_defaultPath; }
    int GetJpgQuality() const { return m_jpgQuality; }
    bool IsAutoSave() const { return m_autoSave; }
    std::wstring GetAutoSavePath() const { return m_autoSavePath; }

    /**
     * 获取自动保存实际使用的目录。
     *
     * @return 自动保存路径为空时回退到默认保存目录。
     */
    std::wstring GetEffectiveAutoSavePath() const;

    std::wstring GetFileNamePrefix() const { return m_fileNamePrefix; }
    std::wstring GetRecordingPath() const { return m_recordingPath; }
    std::wstring GetScrollingCapturePath() const { return m_scrollingCapturePath; }

    void SetDefaultFormat(const std::wstring& format) { m_defaultFormat = format; }
    void SetDefaultPath(const std::wstring& path) { m_defaultPath = path; }
    void SetJpgQuality(int quality) { m_jpgQuality = quality; }
    void SetAutoSave(bool autoSave) { m_autoSave = autoSave; }
    void SetAutoSavePath(const std::wstring& path) { m_autoSavePath = path; }
    void SetFileNamePrefix(const std::wstring& prefix) { m_fileNamePrefix = prefix; }
    void SetRecordingPath(const std::wstring& path) { m_recordingPath = path; }
    void SetScrollingCapturePath(const std::wstring& path) { m_scrollingCapturePath = path; }

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

    /**
     * 设置开机自启开关，并同步更新当前用户 Run 注册表项。
     *
     * @param autoStart true 写入 SnipX 启动项；false 删除启动项。
     * @note 即使注册表写入失败，也会更新内存中的开关状态。
     */
    void SetAutoStart(bool autoStart);

private:
    /**
     * 解析并确保配置文件目录存在。
     *
     * @return `%APPDATA%\\SnipX\\config.ini`；失败时返回空字符串。
     */
    std::wstring GetConfigPath();

    /**
     * 将所有字段重置为内置默认值，并推导图片/录屏相关默认路径。
     */
    void SetDefaultValues();

    // 热键（MAKEWORD 编码：低字节为虚拟键，高字节为修饰键）
    UINT m_hotkey;
    UINT m_fullScreenHotkey;

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
    std::wstring m_recordingPath;
    std::wstring m_scrollingCapturePath;

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
