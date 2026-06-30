// 系统托盘图标实现

#include "TrayIcon.h"
#include "SnipX.h"
#include "resource.h"
#include "Version.h"
#include <vector>


TrayIcon::TrayIcon(SnipXApp* pApp)
    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_visible(false)
{
    // 创建隐藏窗口用于接收托盘消息
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = pApp->GetInstance();
    wc.lpszClassName = L"SnipXTrayWindow";
    RegisterClassExW(&wc);
    
    m_hwnd = CreateWindowExW(0, L"SnipXTrayWindow", L"", 0, 
                            0, 0, 0, 0, HWND_MESSAGE, NULL, 
                            pApp->GetInstance(), this);
    
    // 初始化托盘图标数据
    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    // 使用系统图标作为临时方案
    m_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(m_nid.szTip, L"SnipX - 截图工具");
}

TrayIcon::~TrayIcon()
{
    Hide();
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}

bool TrayIcon::Show()
{
    if (!m_visible)
    {
        m_visible = Shell_NotifyIconW(NIM_ADD, &m_nid) != 0;
    }
    return m_visible;
}

void TrayIcon::Hide()
{
    if (m_visible)
    {
        Shell_NotifyIconW(NIM_DELETE, &m_nid);
        m_visible = false;
    }
}

void TrayIcon::ShowContextMenu()
{
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, IDM_CAPTURE, L"截图 (Alt+X)");
    AppendMenuW(hMenu, MF_STRING, IDM_SCROLL_CAPTURE, L"滚动截图");
    AppendMenuW(hMenu, MF_STRING, IDM_TOGGLE_RECORDING, m_pApp->IsRecording() ? L"停止录屏" : L"开始录屏");
    AppendMenuW(hMenu, MF_STRING, IDM_COLOR_PICKER, L"取色器");
    AppendMenuW(hMenu, MF_STRING, IDM_PIN_CLIPBOARD, L"剪贴板贴图");

    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

    HMENU hHistoryMenu = CreatePopupMenu();
    std::vector<HistoryItem> historyItems = m_pApp->GetConfig()->GetHistoryItems();
    if (historyItems.empty())
    {
        AppendMenuW(hHistoryMenu, MF_STRING | MF_GRAYED, 0, L"暂无历史记录");
    }
    else
    {
        for (size_t i = 0; i < historyItems.size(); i++)
        {
            std::wstring menuText = historyItems[i].timestamp + L"  " + historyItems[i].path;
            AppendMenuW(hHistoryMenu, MF_STRING, IDM_HISTORY_BASE + (UINT)i, menuText.c_str());
        }
        AppendMenuW(hHistoryMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hHistoryMenu, MF_STRING, IDM_HISTORY_CLEAR, L"清空历史记录");
    }
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHistoryMenu, L"历史记录");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_SETTINGS, L"设置");

    AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"关于");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"退出");
    
    // 设置前台窗口以便菜单能正确关闭
    SetForegroundWindow(m_hwnd);
    
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hwnd, NULL);
    
    DestroyMenu(hMenu);
}

LRESULT CALLBACK TrayIcon::TrayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    
    TrayIcon* pThis = (TrayIcon*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if (msg == WM_TRAYICON)
    {
        if (lParam == WM_LBUTTONDBLCLK)
        {
            // 双击托盘图标截图
            if (pThis && pThis->m_pApp)
            {
                pThis->m_pApp->StartCapture();
            }
        }
        else if (lParam == WM_RBUTTONUP)
        {
            // 右键显示菜单
            if (pThis)
            {
                pThis->ShowContextMenu();
            }
        }
        return 0;
    }
    else if (msg == WM_COMMAND)
    {
        if (pThis && pThis->m_pApp)
        {
            UINT commandId = LOWORD(wParam);
            if (commandId >= IDM_HISTORY_BASE && commandId < IDM_HISTORY_BASE + 50)
            {
                std::vector<HistoryItem> historyItems = pThis->m_pApp->GetConfig()->GetHistoryItems();
                UINT index = commandId - IDM_HISTORY_BASE;
                if (index < historyItems.size())
                {
                    ShellExecuteW(hwnd, L"open", historyItems[index].path.c_str(), NULL, NULL, SW_SHOWNORMAL);
                }
                return 0;
            }

            switch (commandId)
            {
            case IDM_CAPTURE:

                pThis->m_pApp->StartCapture();
                break;
            case IDM_SCROLL_CAPTURE:
                pThis->m_pApp->StartScrollingCapture();
                break;
            case IDM_TOGGLE_RECORDING:
                pThis->m_pApp->ToggleRecording();
                break;
            case IDM_COLOR_PICKER:
                pThis->m_pApp->StartColorPicker();
                break;
            case IDM_PIN_CLIPBOARD:

                pThis->m_pApp->CreatePinWindowFromClipboard(hwnd);
                break;
            case IDM_SETTINGS:
                pThis->m_pApp->ShowSettings();
                break;
            case IDM_HISTORY_CLEAR:
                pThis->m_pApp->GetConfig()->ClearHistoryItems();
                pThis->m_pApp->GetConfig()->Save();
                break;
            case IDM_ABOUT:
            {
                std::wstring about = L"SnipX ";
                about += GetVersionString();
                about += L"\n极致轻量截图工具\n\n";
                about += L"基于纯 Win32 C++17、GDI+ 和系统 API 开发。\n\n";
                about += L"功能：截图和标注、取色器、贴图、录屏帧序列、保存和复制。\n\n";
                about += L"GitHub：\nhttps://github.com/Vogadero/SnipX\n\n";
                about += L"点击“是”打开 GitHub 仓库，点击“否”关闭。";
                if (MessageBoxW(hwnd, about.c_str(), L"关于 SnipX", MB_ICONINFORMATION | MB_YESNO) == IDYES)
                {
                    ShellExecuteW(hwnd, L"open", L"https://github.com/Vogadero/SnipX", NULL, NULL, SW_SHOWNORMAL);
                }
                break;
            }
            case IDM_EXIT:
                pThis->m_pApp->Exit();
                break;
            }
        }
        return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
