// 系统托盘图标

#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>
#include <shellapi.h>

class SnipXApp;

#define WM_TRAYICON (WM_USER + 2)
#define IDM_CAPTURE 1001
#define IDM_PIN_CLIPBOARD 1002
#define IDM_SCROLL_CAPTURE 1003
#define IDM_TOGGLE_RECORDING 1004
#define IDM_COLOR_PICKER 1005
#define IDM_SETTINGS 1006
#define IDM_ABOUT 1007
#define IDM_EXIT 1008

#define IDM_HISTORY_BASE 1100
#define IDM_HISTORY_CLEAR 1199


class TrayIcon
{
public:
    TrayIcon(SnipXApp* pApp);
    ~TrayIcon();
    
    bool Show();
    void Hide();
    
private:
    static LRESULT CALLBACK TrayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void ShowContextMenu();
    
    SnipXApp* m_pApp;
    HWND m_hwnd;
    NOTIFYICONDATAW m_nid;
    bool m_visible;
};

#endif // TRAYICON_H
