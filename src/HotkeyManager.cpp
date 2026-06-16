// 热键管理器实现

#include "HotkeyManager.h"
#include "SnipX.h"

namespace
{
    /**
     * 将配置中使用的 HOTKEYF_* 组合键转换为 RegisterHotKey 所需修饰键。
     *
     * @param hotkey MAKEWORD 形式的热键配置。
     * @return RegisterHotKey 使用的 MOD_* 修饰键。
     */
    UINT GetRegisterModifiers(UINT hotkey)
    {
        BYTE modifiers = HIBYTE(hotkey);
        UINT fsModifiers = 0;
        if (modifiers & HOTKEYF_ALT) fsModifiers |= MOD_ALT;
        if (modifiers & HOTKEYF_CONTROL) fsModifiers |= MOD_CONTROL;
        if (modifiers & HOTKEYF_SHIFT) fsModifiers |= MOD_SHIFT;
        return fsModifiers;
    }
}

HotkeyManager::HotkeyManager(SnipXApp* pApp)

    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_hotkey(0)
    , m_registered(false)
{
    // 创建隐藏窗口用于接收热键消息
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = HotkeyWndProc;
    wc.hInstance = pApp->GetInstance();
    wc.lpszClassName = L"SnipXHotkeyWindow";
    RegisterClassExW(&wc);
    
    m_hwnd = CreateWindowExW(0, L"SnipXHotkeyWindow", L"", 0, 
                            0, 0, 0, 0, HWND_MESSAGE, NULL, 
                            pApp->GetInstance(), this);
}

HotkeyManager::~HotkeyManager()
{
    UnregisterHotkey();
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}

bool HotkeyManager::RegisterHotkey(UINT hotkey)
{
    if (m_registered)
    {
        UnregisterHotkey();
    }
    
    m_hotkey = hotkey;
    UINT screenshotModifiers = GetRegisterModifiers(hotkey);
    UINT fullScreenHotkey = m_pApp->GetConfig()->GetFullScreenHotkey();
    UINT fullScreenModifiers = GetRegisterModifiers(fullScreenHotkey);

    bool screenshotRegistered = ::RegisterHotKey(m_hwnd, HOTKEY_ID_SCREENSHOT, screenshotModifiers, LOBYTE(hotkey)) != 0;
    bool fullScreenRegistered = ::RegisterHotKey(m_hwnd, HOTKEY_ID_FULLSCREEN, fullScreenModifiers, LOBYTE(fullScreenHotkey)) != 0;

    m_registered = screenshotRegistered || fullScreenRegistered;
    return m_registered;
}

void HotkeyManager::UnregisterHotkey()
{
    if (m_registered)
    {
        ::UnregisterHotKey(m_hwnd, HOTKEY_ID_SCREENSHOT);
        ::UnregisterHotKey(m_hwnd, HOTKEY_ID_FULLSCREEN);
        m_registered = false;
    }
}


LRESULT CALLBACK HotkeyManager::HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    
    HotkeyManager* pThis = (HotkeyManager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if (msg == WM_HOTKEY)
    {
        if (pThis && pThis->m_pApp)
        {
            if (wParam == HOTKEY_ID_FULLSCREEN)
            {
                pThis->m_pApp->StartFullScreenCapture();
            }
            else if (wParam == HOTKEY_ID_SCREENSHOT)
            {
                if (GetKeyState(VK_SHIFT) < 0)
                {
                    pThis->m_pApp->StartLastSelectionCapture();
                }
                else
                {
                    pThis->m_pApp->StartCapture();
                }
            }

        }
        return 0;
    }

    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
