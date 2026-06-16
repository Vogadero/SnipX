// 贴图窗口实现

#include "PinWindow.h"
#include "SnipX.h"
#include "Localization.h"
#include <algorithm>
#include <cstdlib>
#include <cwchar>
#include <shlobj.h>
#include <shlwapi.h>
#include <windowsx.h>

#pragma comment(lib, "shlwapi.lib")

namespace
{
    const int MIN_PIN_SIZE = 50;
    const int MAX_PIN_SIZE = 4000;
    const double PIN_ZOOM_FACTOR = 1.1;
}

#define IDM_PIN_CLOSE 2001
#define IDM_PIN_OPACITY_100 2010
#define IDM_PIN_OPACITY_75 2011
#define IDM_PIN_OPACITY_50 2012
#define IDM_PIN_OPACITY_25 2013
#define IDM_PIN_MOUSE_THROUGH 2020

PinWindow::PinWindow(SnipXApp* pApp, Bitmap* pBitmap)
    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_pBitmap(pBitmap)
    , m_dragging(false)
    , m_opacity(255)
    , m_mouseThrough(false)
    , m_width(100)
    , m_height(100)
    , m_originalWidth(100)
    , m_originalHeight(100)
    , m_scale(1.0)
{
    m_dragStart = { 0, 0 };
    m_windowPos = { 0, 0 };
    
    if (m_pBitmap)
    {
        m_originalWidth = static_cast<int>(m_pBitmap->GetWidth());
        m_originalHeight = static_cast<int>(m_pBitmap->GetHeight());
        m_width = m_originalWidth;
        m_height = m_originalHeight;
    }
    else
    {
        m_width = 100;
        m_height = 100;
        m_originalWidth = 100;
        m_originalHeight = 100;
    }
}

PinWindow::~PinWindow()
{
    Close();
    if (m_pBitmap)
    {
        delete m_pBitmap;
        m_pBitmap = nullptr;
    }
}

void PinWindow::Show()
{
    CreatePinWindow();
}

void PinWindow::Close()
{
    DestroyPinWindow();
}

void PinWindow::CreatePinWindow()
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = PinWndProc;
    wc.hInstance = m_pApp->GetInstance();
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"SnipXPinWindow";
    RegisterClassExW(&wc);
    
    // 居中显示
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - m_width) / 2;
    int y = (screenHeight - m_height) / 2;
    
    m_windowPos.x = x;
    m_windowPos.y = y;
    
    m_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                            L"SnipXPinWindow", L"SnipX 贴图",
                            WS_POPUP,
                            x, y, m_width, m_height,
                            NULL, NULL, m_pApp->GetInstance(), this);
    
    SetLayeredWindowAttributes(m_hwnd, 0, m_opacity, LWA_ALPHA);
    
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

void PinWindow::DestroyPinWindow()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

void PinWindow::DrawPin(HDC hdc)
{
    if (!m_pBitmap)
        return;
    
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    
    // 绘制图片
    graphics.DrawImage(m_pBitmap, 0, 0, m_width, m_height);
    
    // 绘制边框（半透明）
    Pen borderPen(Color(100, 255, 255, 255), 1);
    graphics.DrawRectangle(&borderPen, 0, 0, m_width - 1, m_height - 1);
}

void PinWindow::ShowContextMenu()
{
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = CreatePopupMenu();
    
    // 透明度子菜单
    HMENU hOpacityMenu = CreatePopupMenu();
    AppendMenuW(hOpacityMenu, MF_STRING | (m_opacity == 255 ? MF_CHECKED : 0), IDM_PIN_OPACITY_100, L"100%");
    AppendMenuW(hOpacityMenu, MF_STRING | (m_opacity == 191 ? MF_CHECKED : 0), IDM_PIN_OPACITY_75, L"75%");
    AppendMenuW(hOpacityMenu, MF_STRING | (m_opacity == 127 ? MF_CHECKED : 0), IDM_PIN_OPACITY_50, L"50%");
    AppendMenuW(hOpacityMenu, MF_STRING | (m_opacity == 63 ? MF_CHECKED : 0), IDM_PIN_OPACITY_25, L"25%");
    
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hOpacityMenu, L"透明度");
    AppendMenuW(hMenu, MF_STRING | (m_mouseThrough ? MF_CHECKED : 0), IDM_PIN_MOUSE_THROUGH, L"鼠标穿透");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_PIN_CLOSE, L"关闭");
    
    SetForegroundWindow(m_hwnd);
    
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hwnd, NULL);
    
    DestroyMenu(hOpacityMenu);
    DestroyMenu(hMenu);
}

void PinWindow::SetOpacity(int opacity)
{
    m_opacity = opacity;
    if (m_hwnd)
    {
        SetLayeredWindowAttributes(m_hwnd, 0, m_opacity, LWA_ALPHA);
    }
}

void PinWindow::SetMouseThrough(bool through)
{
    m_mouseThrough = through;
    if (m_hwnd)
    {
        LONG exStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
        if (through)
        {
            exStyle |= WS_EX_TRANSPARENT;
        }
        else
        {
            exStyle &= ~WS_EX_TRANSPARENT;
        }
        SetWindowLong(m_hwnd, GWL_EXSTYLE, exStyle);
    }
}

void PinWindow::ZoomByWheelDelta(short wheelDelta)
{
    if (wheelDelta == 0 || m_originalWidth <= 0 || m_originalHeight <= 0)
        return;

    double nextScale = wheelDelta > 0 ? m_scale * PIN_ZOOM_FACTOR : m_scale / PIN_ZOOM_FACTOR;
    ApplyScale(nextScale);
}

void PinWindow::ApplyScale(double scale)
{
    if (m_originalWidth <= 0 || m_originalHeight <= 0)
        return;

    int maxWidth = (std::max)(MIN_PIN_SIZE, (std::min)(MAX_PIN_SIZE, GetSystemMetrics(SM_CXVIRTUALSCREEN) * 3));
    int maxHeight = (std::max)(MIN_PIN_SIZE, (std::min)(MAX_PIN_SIZE, GetSystemMetrics(SM_CYVIRTUALSCREEN) * 3));

    int newWidth = ClampSize(static_cast<int>(m_originalWidth * scale + 0.5), MIN_PIN_SIZE, maxWidth);
    int newHeight = ClampSize(static_cast<int>(m_originalHeight * scale + 0.5), MIN_PIN_SIZE, maxHeight);

    double widthScale = static_cast<double>(newWidth) / m_originalWidth;
    double heightScale = static_cast<double>(newHeight) / m_originalHeight;
    m_scale = (std::min)(widthScale, heightScale);

    m_width = (std::max)(1, static_cast<int>(m_originalWidth * m_scale + 0.5));
    m_height = (std::max)(1, static_cast<int>(m_originalHeight * m_scale + 0.5));

    if (m_hwnd)
    {
        SetWindowPos(m_hwnd, NULL, 0, 0, m_width, m_height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

int PinWindow::ClampSize(int value, int minValue, int maxValue) const
{
    return (std::max)(minValue, (std::min)(value, maxValue));
}

void PinWindow::CopyToClipboard(HWND hwndOwner)
{
    if (!m_pBitmap)
        return;

    HBITMAP hBitmap = NULL;
    m_pBitmap->GetHBITMAP(Color(255, 255, 255), &hBitmap);
    if (!hBitmap)
        return;

    if (OpenClipboard(hwndOwner))
    {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();
        MessageBoxW(hwndOwner, L10n(L"已复制贴图到剪贴板！", L"Pinned image copied to clipboard!"), L"SnipX", MB_ICONINFORMATION);
    }
    else
    {
        DeleteObject(hBitmap);
    }
}

void PinWindow::SaveImage(HWND hwndOwner)
{
    if (!m_pBitmap)
        return;

    Config* cfg = m_pApp ? m_pApp->GetConfig() : nullptr;
    WCHAR filename[MAX_PATH] = L"";

    SYSTEMTIME st;
    GetLocalTime(&st);
    const WCHAR* prefix = cfg ? cfg->GetFileNamePrefix().c_str() : L"SnipX";
    swprintf_s(filename, L"%s_pin_%04d%02d%02d_%02d%02d%02d.png",
              prefix, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFilter = L"PNG 图片 (*.png)\0*.png\0所有文件 (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    if (cfg)
    {
        ofn.lpstrInitialDir = cfg->GetDefaultPath().c_str();
    }
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"png";

    if (!GetSaveFileNameW(&ofn))
        return;

    CLSID encoderClsid;
    UINT num = 0;
    UINT size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (!pImageCodecInfo)
        return;

    bool saved = false;
    if (GetImageEncoders(num, size, pImageCodecInfo) == Ok)
    {
        for (UINT i = 0; i < num; i++)
        {
            if (wcscmp(pImageCodecInfo[i].MimeType, L"image/png") == 0)
            {
                encoderClsid = pImageCodecInfo[i].Clsid;
                saved = m_pBitmap->Save(filename, &encoderClsid, NULL) == Ok;
                break;
            }
        }
    }

    free(pImageCodecInfo);

    if (!saved)
    {
        MessageBoxW(hwndOwner, L10n(L"保存贴图失败，请检查路径。", L"Failed to save pinned image. Please check the path."), L"SnipX", MB_ICONERROR);
        return;
    }

    if (cfg)
    {
        cfg->AddHistoryItem(filename);
        cfg->Save();
    }
    MessageBoxW(hwndOwner, L10n(L"贴图保存成功！", L"Pinned image saved!"), L"SnipX", MB_ICONINFORMATION);
}

LRESULT CALLBACK PinWindow::PinWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    
    PinWindow* pThis = (PinWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        RECT rc;
        GetClientRect(hwnd, &rc);
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        
        if (pThis)
        {
            pThis->DrawPin(hdcMem);
        }
        
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
        
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        
        EndPaint(hwnd, &ps);
        return 0;
    }
    
    case WM_LBUTTONDOWN:
    {
        if (pThis)
        {
            pThis->m_dragging = true;
            pThis->m_dragStart.x = GET_X_LPARAM(lParam);
            pThis->m_dragStart.y = GET_Y_LPARAM(lParam);
            SetCapture(hwnd);
        }
        return 0;
    }
    
    case WM_MOUSEMOVE:
    {
        if (pThis && pThis->m_dragging)
        {
            POINT pt;
            GetCursorPos(&pt);
            
            int newX = pt.x - pThis->m_dragStart.x;
            int newY = pt.y - pThis->m_dragStart.y;
            
            SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            
            pThis->m_windowPos.x = newX;
            pThis->m_windowPos.y = newY;
        }
        return 0;
    }
    
    case WM_LBUTTONUP:
    {
        if (pThis && pThis->m_dragging)
        {
            pThis->m_dragging = false;
            ReleaseCapture();
        }
        return 0;
    }
    
    case WM_RBUTTONUP:
    {
        if (pThis)
        {
            pThis->ShowContextMenu();
        }
        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        if (pThis && !pThis->m_dragging)
        {
            pThis->ZoomByWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam));
        }
        return 0;
    }

    case WM_KEYDOWN:
    {
        if (pThis)
        {
            bool ctrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            if (wParam == VK_ESCAPE)
            {
                pThis->Close();
                return 0;
            }
            if (ctrlDown && wParam == 'C')
            {
                pThis->CopyToClipboard(hwnd);
                return 0;
            }
            if (ctrlDown && wParam == 'S')
            {
                pThis->SaveImage(hwnd);
                return 0;
            }
        }
        break;
    }

    case WM_COMMAND:
    {
        if (pThis)
        {
            switch (LOWORD(wParam))
            {
            case IDM_PIN_CLOSE:
                pThis->Close();
                break;
            case IDM_PIN_OPACITY_100:
                pThis->SetOpacity(255);
                break;
            case IDM_PIN_OPACITY_75:
                pThis->SetOpacity(191);
                break;
            case IDM_PIN_OPACITY_50:
                pThis->SetOpacity(127);
                break;
            case IDM_PIN_OPACITY_25:
                pThis->SetOpacity(63);
                break;
            case IDM_PIN_MOUSE_THROUGH:
                pThis->SetMouseThrough(!pThis->m_mouseThrough);
                break;
            }
        }
        return 0;
    }
    
    case WM_DESTROY:
    {
        return 0;
    }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
