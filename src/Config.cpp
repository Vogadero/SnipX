// 配置管理类实现

#include "Config.h"
#include <shlobj.h>
#include <fstream>
#include <sstream>

namespace
{
    const int DEFAULT_HISTORY_LIMIT = 10;

    /**
     * 生成历史记录显示用的本地时间戳。
     *
     * @return 格式为 yyyy-MM-dd HH:mm:ss 的时间字符串。
     */
    std::wstring CreateHistoryTimestamp()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        WCHAR timestamp[32] = { 0 };
        swprintf_s(timestamp, L"%04d-%02d-%02d %02d:%02d:%02d",
                   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        return timestamp;
    }

    /**
     * 向 INI 写入整数配置项。
     */
    void WriteIniInt(const wchar_t* section, const wchar_t* key, int value, const std::wstring& path)
    {
        WCHAR buffer[32];
        _itow_s(value, buffer, 10);
        WritePrivateProfileStringW(section, key, buffer, path.c_str());
    }

    /**
     * 向 INI 写入布尔配置项（1/0）。
     */
    void WriteIniBool(const wchar_t* section, const wchar_t* key, bool value, const std::wstring& path)
    {
        WritePrivateProfileStringW(section, key, value ? L"1" : L"0", path.c_str());
    }
}


Config::Config()
{
    SetDefaultValues();
}

Config::~Config()
{
}

void Config::SetDefaultValues()
{
    m_hotkey = MAKEWORD('X', HOTKEYF_ALT);
    m_fullScreenHotkey = MAKEWORD('F', HOTKEYF_ALT);
    m_hasLastSelection = false;
    SetRectEmpty(&m_lastSelection);
    m_historyLimit = DEFAULT_HISTORY_LIMIT;
    m_historyItems.clear();
    m_defaultFormat = L"png";
    m_jpgQuality = 90;
    m_autoSave = false;
    m_fileNamePrefix = L"截图";
    m_trayIconVisible = true;
    m_startupMode = STARTUP_TRAY;
    m_exitMode = EXIT_TRAY;
    m_defaultColor = RGB(255, 0, 0);
    m_defaultStrokeWidth = 2;
    m_autoStart = false;
    
    // 优先把默认输出目录放在“图片”下，方便用户直接找到
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYPICTURES, NULL, 0, path)))
    {
        m_defaultPath = path;
        m_defaultPath += L"\\Screenshots";
        m_autoSavePath = m_defaultPath;
        m_recordingPath = path;
        m_recordingPath += L"\\SnipXRecordings";
        m_scrollingCapturePath = path;
        m_scrollingCapturePath += L"\\SnipXScrollingCaptures";
    }
}

/**
 * 获取当前自动保存应使用的目录。
 *
 * @return 自动保存路径为空时回退到默认保存目录。
 */
std::wstring Config::GetEffectiveAutoSavePath() const
{
    return m_autoSavePath.empty() ? m_defaultPath : m_autoSavePath;
}

std::wstring Config::GetConfigPath()
{
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path)))
    {
        std::wstring configPath = path;
        configPath += L"\\SnipX";

        // 确保配置目录存在，避免首次启动时 INI 无法创建
        CreateDirectoryW(configPath.c_str(), NULL);

        configPath += L"\\config.ini";
        return configPath;
    }
    return L"";
}

bool Config::Load()
{
    std::wstring configPath = GetConfigPath();
    if (configPath.empty())
        return false;

    // 分段读取 INI：缺失项保留构造时默认值，兼容旧配置缺键
    WCHAR buffer[MAX_PATH];

    // 热键
    UINT hotkey = GetPrivateProfileIntW(L"Hotkey", L"Key", m_hotkey, configPath.c_str());
    if (hotkey != 0)
        m_hotkey = hotkey;

    UINT fullScreenHotkey = GetPrivateProfileIntW(L"Hotkey", L"FullScreenKey", m_fullScreenHotkey, configPath.c_str());
    if (fullScreenHotkey != 0)
        m_fullScreenHotkey = fullScreenHotkey;

    // 上次选区：标记为有效但矩形为空时，按无选区处理
    m_hasLastSelection = GetPrivateProfileIntW(L"Selection", L"HasLastSelection", m_hasLastSelection, configPath.c_str()) != 0;
    m_lastSelection.left = GetPrivateProfileIntW(L"Selection", L"Left", m_lastSelection.left, configPath.c_str());
    m_lastSelection.top = GetPrivateProfileIntW(L"Selection", L"Top", m_lastSelection.top, configPath.c_str());
    m_lastSelection.right = GetPrivateProfileIntW(L"Selection", L"Right", m_lastSelection.right, configPath.c_str());
    m_lastSelection.bottom = GetPrivateProfileIntW(L"Selection", L"Bottom", m_lastSelection.bottom, configPath.c_str());
    if (m_hasLastSelection && IsRectEmpty(&m_lastSelection))
    {
        m_hasLastSelection = false;
    }

    // 历史记录
    m_historyLimit = GetPrivateProfileIntW(L"History", L"Limit", m_historyLimit, configPath.c_str());
    int historyCount = GetPrivateProfileIntW(L"History", L"Count", 0, configPath.c_str());
    m_historyItems.clear();
    for (int i = 0; i < historyCount && i < m_historyLimit; i++)
    {
        WCHAR key[32] = { 0 };
        WCHAR pathBuffer[MAX_PATH] = { 0 };
        WCHAR timeBuffer[64] = { 0 };

        swprintf_s(key, L"Path%d", i);
        GetPrivateProfileStringW(L"History", key, L"", pathBuffer, MAX_PATH, configPath.c_str());
        swprintf_s(key, L"Time%d", i);
        GetPrivateProfileStringW(L"History", key, L"", timeBuffer, 64, configPath.c_str());

        // 跳过空路径槽位，兼容旧配置中的空洞项
        if (pathBuffer[0] != L'\0')
        {
            HistoryItem item;
            item.path = pathBuffer;
            item.timestamp = timeBuffer;
            m_historyItems.push_back(item);
        }
    }
    
    // 保存格式

    GetPrivateProfileStringW(L"Save", L"DefaultFormat", m_defaultFormat.c_str(), 
                            buffer, MAX_PATH, configPath.c_str());
    m_defaultFormat = buffer;
    
    GetPrivateProfileStringW(L"Save", L"DefaultPath", m_defaultPath.c_str(), 
                            buffer, MAX_PATH, configPath.c_str());
    m_defaultPath = buffer;
    
    m_jpgQuality = GetPrivateProfileIntW(L"Save", L"JpgQuality", m_jpgQuality, configPath.c_str());
    m_autoSave = GetPrivateProfileIntW(L"Save", L"AutoSave", m_autoSave, configPath.c_str()) != 0;
    
    GetPrivateProfileStringW(L"Save", L"AutoSavePath", m_autoSavePath.c_str(), 
                            buffer, MAX_PATH, configPath.c_str());
    m_autoSavePath = buffer;
    
    GetPrivateProfileStringW(L"Save", L"FileNamePrefix", m_fileNamePrefix.c_str(),
                            buffer, MAX_PATH, configPath.c_str());
    m_fileNamePrefix = buffer;

    GetPrivateProfileStringW(L"Save", L"RecordingPath", m_recordingPath.c_str(),
                            buffer, MAX_PATH, configPath.c_str());
    m_recordingPath = buffer;

    GetPrivateProfileStringW(L"Save", L"ScrollingCapturePath", m_scrollingCapturePath.c_str(),
                            buffer, MAX_PATH, configPath.c_str());
    m_scrollingCapturePath = buffer;

    // 外观
    m_trayIconVisible = GetPrivateProfileIntW(L"Appearance", L"TrayIconVisible", 
                                              m_trayIconVisible, configPath.c_str()) != 0;
    m_startupMode = (StartupMode)GetPrivateProfileIntW(L"Appearance", L"StartupMode", 
                                                       m_startupMode, configPath.c_str());
    m_exitMode = (ExitMode)GetPrivateProfileIntW(L"Appearance", L"ExitMode", 
                                                 m_exitMode, configPath.c_str());
    
    // 标注
    m_defaultColor = GetPrivateProfileIntW(L"Annotation", L"DefaultColor", 
                                          m_defaultColor, configPath.c_str());
    m_defaultStrokeWidth = GetPrivateProfileIntW(L"Annotation", L"DefaultStrokeWidth", 
                                                 m_defaultStrokeWidth, configPath.c_str());
    
    // 启动
    m_autoStart = GetPrivateProfileIntW(L"Startup", L"AutoStart", m_autoStart, configPath.c_str()) != 0;
    
    return true;
}

bool Config::Save()
{
    std::wstring configPath = GetConfigPath();
    if (configPath.empty())
        return false;

    // 热键
    WriteIniInt(L"Hotkey", L"Key", m_hotkey, configPath);
    WriteIniInt(L"Hotkey", L"FullScreenKey", m_fullScreenHotkey, configPath);

    // 上次选区
    WriteIniBool(L"Selection", L"HasLastSelection", m_hasLastSelection, configPath);
    WriteIniInt(L"Selection", L"Left", m_lastSelection.left, configPath);
    WriteIniInt(L"Selection", L"Top", m_lastSelection.top, configPath);
    WriteIniInt(L"Selection", L"Right", m_lastSelection.right, configPath);
    WriteIniInt(L"Selection", L"Bottom", m_lastSelection.bottom, configPath);

    // 历史记录：固定写出 Limit 个槽位，空槽写空串以清理旧键
    WriteIniInt(L"History", L"Limit", m_historyLimit, configPath);
    WriteIniInt(L"History", L"Count", (int)m_historyItems.size(), configPath);
    for (int i = 0; i < m_historyLimit; i++)
    {
        const bool hasItem = i < (int)m_historyItems.size();
        WCHAR key[32] = { 0 };
        swprintf_s(key, L"Path%d", i);
        WritePrivateProfileStringW(L"History", key, hasItem ? m_historyItems[i].path.c_str() : L"", configPath.c_str());
        swprintf_s(key, L"Time%d", i);
        WritePrivateProfileStringW(L"History", key, hasItem ? m_historyItems[i].timestamp.c_str() : L"", configPath.c_str());
    }

    // 保存
    WritePrivateProfileStringW(L"Save", L"DefaultFormat", m_defaultFormat.c_str(), configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"DefaultPath", m_defaultPath.c_str(), configPath.c_str());
    WriteIniInt(L"Save", L"JpgQuality", m_jpgQuality, configPath);
    WriteIniBool(L"Save", L"AutoSave", m_autoSave, configPath);
    WritePrivateProfileStringW(L"Save", L"AutoSavePath", m_autoSavePath.c_str(), configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"FileNamePrefix", m_fileNamePrefix.c_str(), configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"RecordingPath", m_recordingPath.c_str(), configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"ScrollingCapturePath", m_scrollingCapturePath.c_str(), configPath.c_str());

    // 外观
    WriteIniBool(L"Appearance", L"TrayIconVisible", m_trayIconVisible, configPath);
    WriteIniInt(L"Appearance", L"StartupMode", m_startupMode, configPath);
    WriteIniInt(L"Appearance", L"ExitMode", m_exitMode, configPath);

    // 标注
    WriteIniInt(L"Annotation", L"DefaultColor", m_defaultColor, configPath);
    WriteIniInt(L"Annotation", L"DefaultStrokeWidth", m_defaultStrokeWidth, configPath);

    // 启动
    WriteIniBool(L"Startup", L"AutoStart", m_autoStart, configPath);

    return true;
}

void Config::SetLastSelection(const RECT& rect)
{
    m_lastSelection = rect;
    m_hasLastSelection = !IsRectEmpty(&m_lastSelection);
}

void Config::AddHistoryItem(const std::wstring& path)
{
    if (path.empty())
        return;

    // 相同路径先移除，再插到头部，保证最近使用优先
    for (auto it = m_historyItems.begin(); it != m_historyItems.end(); ++it)
    {
        if (it->path == path)
        {
            m_historyItems.erase(it);
            break;
        }
    }

    HistoryItem item;
    item.path = path;
    item.timestamp = CreateHistoryTimestamp();
    m_historyItems.insert(m_historyItems.begin(), item);

    while ((int)m_historyItems.size() > m_historyLimit)
    {
        m_historyItems.pop_back();
    }
}

void Config::ClearHistoryItems()
{
    m_historyItems.clear();
}

void Config::ClearLastSelection()

{
    m_hasLastSelection = false;
    SetRectEmpty(&m_lastSelection);
}

void Config::SetAutoStart(bool autoStart)

{
    m_autoStart = autoStart;
    
    // 同步当前用户 Run 注册表；失败时仍保留内存开关
    HKEY hKey;
    LPCWSTR subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if (autoStart)
        {
            WCHAR exePath[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            RegSetValueExW(hKey, L"SnipX", 0, REG_SZ, (BYTE*)exePath, 
                          (wcslen(exePath) + 1) * sizeof(WCHAR));
        }
        else
        {
            RegDeleteValueW(hKey, L"SnipX");
        }
        RegCloseKey(hKey);
    }
}
