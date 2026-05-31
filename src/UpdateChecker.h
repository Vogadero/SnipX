// 更新检查器
// 检查 GitHub Releases 获取最新版本

#ifndef UPDATE_CHECKER_H
#define UPDATE_CHECKER_H

#include <windows.h>
#include <wininet.h>
#include <string>
#include "Version.h"

#pragma comment(lib, "wininet.lib")

// 更新信息结构
struct UpdateInfo
{
    bool hasUpdate;
    std::wstring latestVersion;
    std::wstring downloadUrl;
    std::wstring releaseNotes;
    std::wstring publishDate;
};

// 更新检查器类
class UpdateChecker
{
private:
    std::wstring m_apiUrl;
    std::wstring m_userAgent;
    
public:
    UpdateChecker()
        : m_apiUrl(L"api.github.com")
        , m_userAgent(L"SnipX-UpdateChecker/1.0")
    {
    }
    
    // 检查更新（同步）
    UpdateInfo CheckForUpdate(const std::wstring& owner, const std::wstring& repo)
    {
        UpdateInfo info = { false };
        
        // 构建 API 路径
        std::wstring path = L"/repos/" + owner + L"/" + repo + L"/releases/latest";
        
        // 打开 Internet 连接
        HINTERNET hInternet = InternetOpenW(m_userAgent.c_str(),
                                            INTERNET_OPEN_TYPE_PRECONFIG,
                                            NULL, NULL, 0);
        if (!hInternet)
            return info;
        
        // 连接到服务器
        HINTERNET hConnect = InternetConnectW(hInternet,
                                               m_apiUrl.c_str(),
                                               INTERNET_DEFAULT_HTTPS_PORT,
                                               NULL, NULL,
                                               INTERNET_SERVICE_HTTP,
                                               0, 0);
        if (!hConnect)
        {
            InternetCloseHandle(hInternet);
            return info;
        }
        
        // 打开请求
        HINTERNET hRequest = HttpOpenRequestW(hConnect,
                                               L"GET",
                                               path.c_str(),
                                               NULL, NULL, NULL,
                                               INTERNET_FLAG_SECURE,
                                               0);
        if (!hRequest)
        {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return info;
        }
        
        // 发送请求
        if (!HttpSendRequestW(hRequest, NULL, 0, NULL, 0))
        {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return info;
        }
        
        // 读取响应
        char buffer[8192];
        DWORD bytesRead = 0;
        std::string response;
        
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        
        // 关闭句柄
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        
        // 解析 JSON 响应（简化版）
        if (!response.empty())
        {
            info = ParseReleaseInfo(response);
        }
        
        return info;
    }
    
    // 异步检查更新
    static DWORD WINAPI CheckUpdateThread(LPVOID param)
    {
        UpdateChecker* checker = (UpdateChecker*)param;
        UpdateInfo info = checker->CheckForUpdate(L"yourusername", L"SnipX");
        
        if (info.hasUpdate)
        {
            // 通知主线程有更新
            // 可以通过 PostMessage 发送自定义消息
        }
        
        return 0;
    }
    
    // 启动异步检查
    void CheckUpdateAsync()
    {
        HANDLE hThread = CreateThread(NULL, 0, CheckUpdateThread, this, 0, NULL);
        if (hThread)
        {
            CloseHandle(hThread);
        }
    }
    
    // 比较版本号
    static bool IsNewerVersion(const std::wstring& version)
    {
        // 解析版本号 "v1.2.3" -> 1, 2, 3
        int major = 0, minor = 0, patch = 0;
        
        const wchar_t* p = version.c_str();
        if (*p == L'v' || *p == L'V')
            p++;
        
        swscanf_s(p, L"%d.%d.%d", &major, &minor, &patch);
        
        // 比较版本
        return IsVersionNewer(major, minor, patch);
    }
    
    // 打开下载页面
    static void OpenDownloadPage(const std::wstring& url)
    {
        ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    
private:
    // 解析 JSON 响应（简化版，仅提取关键字段）
    UpdateInfo ParseReleaseInfo(const std::string& json)
    {
        UpdateInfo info = { false };
        
        // 提取 tag_name
        size_t tagPos = json.find("\"tag_name\"");
        if (tagPos != std::string::npos)
        {
            size_t start = json.find("\"", tagPos + 11);
            size_t end = json.find("\"", start + 1);
            if (start != std::string::npos && end != std::string::npos)
            {
                std::string tag = json.substr(start + 1, end - start - 1);
                info.latestVersion = StringToWString(tag);
                info.hasUpdate = IsNewerVersion(info.latestVersion);
            }
        }
        
        // 提取 html_url
        size_t urlPos = json.find("\"html_url\"");
        if (urlPos != std::string::npos)
        {
            size_t start = json.find("\"", urlPos + 11);
            size_t end = json.find("\"", start + 1);
            if (start != std::string::npos && end != std::string::npos)
            {
                std::string url = json.substr(start + 1, end - start - 1);
                info.downloadUrl = StringToWString(url);
            }
        }
        
        // 提取 body (release notes)
        size_t bodyPos = json.find("\"body\"");
        if (bodyPos != std::string::npos)
        {
            size_t start = json.find("\"", bodyPos + 7);
            size_t end = json.find("\"", start + 1);
            if (start != std::string::npos && end != std::string::npos)
            {
                std::string body = json.substr(start + 1, end - start - 1);
                info.releaseNotes = StringToWString(body);
            }
        }
        
        // 提取 published_at
        size_t datePos = json.find("\"published_at\"");
        if (datePos != std::string::npos)
        {
            size_t start = json.find("\"", datePos + 15);
            size_t end = json.find("\"", start + 1);
            if (start != std::string::npos && end != std::string::npos)
            {
                std::string date = json.substr(start + 1, end - start - 1);
                info.publishDate = StringToWString(date);
            }
        }
        
        return info;
    }
    
    // 字符串转换
    std::wstring StringToWString(const std::string& str)
    {
        if (str.empty())
            return L"";
        
        int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        if (len <= 0)
            return L"";
        
        wchar_t* buffer = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, len);
        
        std::wstring result(buffer);
        delete[] buffer;
        
        return result;
    }
};

#endif // UPDATE_CHECKER_H
