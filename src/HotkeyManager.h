// 热键管理器

#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <windows.h>

class SnipXApp;

#define WM_HOTKEY_PRESSED (WM_USER + 1)
#define HOTKEY_ID_SCREENSHOT 1
#define HOTKEY_ID_FULLSCREEN 2


class HotkeyManager
{
public:
    HotkeyManager(SnipXApp* pApp);
    ~HotkeyManager();
    
    bool RegisterHotkey(UINT hotkey);
    void UnregisterHotkey();
    
private:
    static LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    SnipXApp* m_pApp;
    HWND m_hwnd;
    UINT m_hotkey;
    bool m_registered;
};

#endif // HOTKEYMANAGER_H
