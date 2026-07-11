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

/**
 * 系统托盘图标与右键菜单入口。
 *
 * 负责显示/隐藏托盘图标，并通过菜单触发截图、贴图、录屏、设置等功能。
 */
class TrayIcon
{
public:
    /**
     * 构造托盘图标管理器。
     *
     * @param pApp 所属应用实例。
     */
    TrayIcon(SnipXApp* pApp);
    ~TrayIcon();

    /**
     * 显示托盘图标；首次调用时创建隐藏消息窗口并注册托盘通知。
     *
     * @return 托盘图标注册成功时返回 true。
     */
    bool Show();

    /**
     * 隐藏托盘图标；不会销毁应用其它模块。
     */
    void Hide();

private:
    static LRESULT CALLBACK TrayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * 弹出右键上下文菜单，并根据当前历史记录/录屏状态动态填充项。
     */
    void ShowContextMenu();

    SnipXApp* m_pApp;
    HWND m_hwnd;
    NOTIFYICONDATAW m_nid;
    bool m_visible;
};

#endif // TRAYICON_H
