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
    
    // 获取默认保存路径
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYPICTURES, NULL, 0, path)))
    {
        m_defaultPath = path;
        m_defaultPath += L"\\Screenshots";
        m_autoSavePath = m_defaultPath;
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
        
        // 确保目录存在
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
    
    // 使用Windows INI API读取配置
    WCHAR buffer[MAX_PATH];
    
    // 热键
    UINT hotkey = GetPrivateProfileIntW(L"Hotkey", L"Key", m_hotkey, configPath.c_str());
    if (hotkey != 0)
        m_hotkey = hotkey;

    UINT fullScreenHotkey = GetPrivateProfileIntW(L"Hotkey", L"FullScreenKey", m_fullScreenHotkey, configPath.c_str());
    if (fullScreenHotkey != 0)
        m_fullScreenHotkey = fullScreenHotkey;

    // 上次选区
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
    
    WCHAR buffer[32];
    
    // 热键
    _itow_s(m_hotkey, buffer, 10);
    WritePrivateProfileStringW(L"Hotkey", L"Key", buffer, configPath.c_str());
    _itow_s(m_fullScreenHotkey, buffer, 10);
    WritePrivateProfileStringW(L"Hotkey", L"FullScreenKey", buffer, configPath.c_str());

    // 上次选区
    WritePrivateProfileStringW(L"Selection", L"HasLastSelection", m_hasLastSelection ? L"1" : L"0", configPath.c_str());
    _itow_s(m_lastSelection.left, buffer, 10);
    WritePrivateProfileStringW(L"Selection", L"Left", buffer, configPath.c_str());
    _itow_s(m_lastSelection.top, buffer, 10);
    WritePrivateProfileStringW(L"Selection", L"Top", buffer, configPath.c_str());
    _itow_s(m_lastSelection.right, buffer, 10);
    WritePrivateProfileStringW(L"Selection", L"Right", buffer, configPath.c_str());
    _itow_s(m_lastSelection.bottom, buffer, 10);
    WritePrivateProfileStringW(L"Selection", L"Bottom", buffer, configPath.c_str());

    // 历史记录
    _itow_s(m_historyLimit, buffer, 10);
    WritePrivateProfileStringW(L"History", L"Limit", buffer, configPath.c_str());
    _itow_s((int)m_historyItems.size(), buffer, 10);
    WritePrivateProfileStringW(L"History", L"Count", buffer, configPath.c_str());
    for (int i = 0; i < m_historyLimit; i++)
    {
        WCHAR key[32] = { 0 };
        swprintf_s(key, L"Path%d", i);
        WritePrivateProfileStringW(L"History", key, i < (int)m_historyItems.size() ? m_historyItems[i].path.c_str() : L"", configPath.c_str());
        swprintf_s(key, L"Time%d", i);
        WritePrivateProfileStringW(L"History", key, i < (int)m_historyItems.size() ? m_historyItems[i].timestamp.c_str() : L"", configPath.c_str());
    }
    
    // 保存

    WritePrivateProfileStringW(L"Save", L"DefaultFormat", m_defaultFormat.c_str(), configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"DefaultPath", m_defaultPath.c_str(), configPath.c_str());
    
    _itow_s(m_jpgQuality, buffer, 10);
    WritePrivateProfileStringW(L"Save", L"JpgQuality", buffer, configPath.c_str());
    
    WritePrivateProfileStringW(L"Save", L"AutoSave", m_autoSave ? L"1" : L"0", configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"AutoSavePath", m_autoSavePath.c_str(), configPath.c_str());
    WritePrivateProfileStringW(L"Save", L"FileNamePrefix", m_fileNamePrefix.c_str(), configPath.c_str());
    
    // 外观
    WritePrivateProfileStringW(L"Appearance", L"TrayIconVisible", 
                              m_trayIconVisible ? L"1" : L"0", configPath.c_str());
    _itow_s(m_startupMode, buffer, 10);
    WritePrivateProfileStringW(L"Appearance", L"StartupMode", buffer, configPath.c_str());
    _itow_s(m_exitMode, buffer, 10);
    WritePrivateProfileStringW(L"Appearance", L"ExitMode", buffer, configPath.c_str());
    
    // 标注
    _itow_s(m_defaultColor, buffer, 10);
    WritePrivateProfileStringW(L"Annotation", L"DefaultColor", buffer, configPath.c_str());
    _itow_s(m_defaultStrokeWidth, buffer, 10);
    WritePrivateProfileStringW(L"Annotation", L"DefaultStrokeWidth", buffer, configPath.c_str());
    
    // 启动
    WritePrivateProfileStringW(L"Startup", L"AutoStart", m_autoStart ? L"1" : L"0", configPath.c_str());
    
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
    
    // 设置注册表开机自启
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
