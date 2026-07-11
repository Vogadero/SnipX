// 热键管理器

#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <windows.h>

class SnipXApp;

#define WM_HOTKEY_PRESSED (WM_USER + 1)
#define HOTKEY_ID_SCREENSHOT 1
#define HOTKEY_ID_FULLSCREEN 2

/**
 * 管理全局截图热键的注册、接收与分发。
 *
 * 内部创建隐藏窗口接收 WM_HOTKEY，再通知 SnipXApp 进入对应截图流程。
 */
class HotkeyManager
{
public:
    /**
     * 构造热键管理器。
     *
     * @param pApp 所属应用实例。
     */
    HotkeyManager(SnipXApp* pApp);
    ~HotkeyManager();

    /**
     * 注册区域截图热键，并在实现中一并注册全屏热键。
     *
     * @param hotkey MAKEWORD(vk, modifiers) 编码的热键值。
     * @return 热键注册成功时返回 true。
     * @note 重复调用前应先 UnregisterHotkey，避免句柄/热键泄漏。
     */
    bool RegisterHotkey(UINT hotkey);

    /**
     * 注销已注册的全局热键并释放相关资源。
     */
    void UnregisterHotkey();

private:
    static LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    SnipXApp* m_pApp;
    HWND m_hwnd;
    UINT m_hotkey;
    bool m_registered;
};

#endif // HOTKEYMANAGER_H
