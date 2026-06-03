// 取色器实现

#include "ColorPicker.h"
#include "SnipX.h"
#include <stdio.h>
#include <algorithm>
#include <windowsx.h>
#include <math.h>

ColorPicker::ColorPicker(SnipXApp* pApp)
    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_picking(false)
    , m_pScreenBitmap(nullptr)
    , m_displayMode(0)
{
    m_currentPoint = { 0, 0 };
    m_currentColor = Color(0, 0, 0);
}

ColorPicker::~ColorPicker()
{
    if (m_pScreenBitmap) delete m_pScreenBitmap;
}

void ColorPicker::Start()
{
    if (m_picking)
        return;
    
    m_picking = true;
    
    // 捕获屏幕
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, screenX, screenY, SRCCOPY);
    
    if (m_pScreenBitmap)
        delete m_pScreenBitmap;
    m_pScreenBitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
    
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    
    // 创建覆盖窗口
    CreatePickerWindow();
}

void ColorPicker::Cancel()
{
    m_picking = false;
    DestroyPickerWindow();
}

void ColorPicker::CreatePickerWindow()
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = PickerWndProc;
    wc.hInstance = m_pApp->GetInstance();
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.lpszClassName = L"SnipXColorPickerWindow";
    RegisterClassExW(&wc);
    
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    m_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                            L"SnipXColorPickerWindow", L"",
                            WS_POPUP,
                            screenX, screenY, screenWidth, screenHeight,
                            NULL, NULL, m_pApp->GetInstance(), this);
    
    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void ColorPicker::DestroyPickerWindow()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

void ColorPicker::UpdateColor(POINT pt)
{
    m_currentPoint = pt;
    
    if (!m_pScreenBitmap)
        return;
    
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    
    int x = pt.x - screenX;
    int y = pt.y - screenY;
    
    if (x >= 0 && y >= 0 && x < m_pScreenBitmap->GetWidth() && y < m_pScreenBitmap->GetHeight())
    {
        m_pScreenBitmap->GetPixel(x, y, &m_currentColor);
    }
    
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void ColorPicker::CopyColorToClipboard(int format)
{
    WCHAR colorText[128];

    BYTE red = m_currentColor.GetR();
    BYTE green = m_currentColor.GetG();
    BYTE blue = m_currentColor.GetB();
    float r = red / 255.0f;
    float g = green / 255.0f;
    float b = blue / 255.0f;
    float maxVal = (std::max)((std::max)(r, g), b);
    float minVal = (std::min)((std::min)(r, g), b);
    float delta = maxVal - minVal;
    float hue = 0.0f;
    float hsvSaturation = maxVal == 0.0f ? 0.0f : delta / maxVal;
    float value = maxVal;
    float lightness = (maxVal + minVal) / 2.0f;
    float hslSaturation = 0.0f;

    if (delta > 0.0f)
    {
        hslSaturation = delta / (1.0f - fabsf(2.0f * lightness - 1.0f));
        if (maxVal == r)
            hue = 60.0f * (((g - b) / delta) + (g < b ? 6.0f : 0.0f));
        else if (maxVal == g)
            hue = 60.0f * (((b - r) / delta) + 2.0f);
        else
            hue = 60.0f * (((r - g) / delta) + 4.0f);
    }

    switch (format)
    {
    case 0: // HEX
        swprintf_s(colorText, L"#%02X%02X%02X", red, green, blue);
        break;
    case 1: // RGB
        swprintf_s(colorText, L"rgb(%d, %d, %d)", red, green, blue);
        break;
    case 2: // HSL
        swprintf_s(colorText, L"hsl(%d, %d%%, %d%%)", (int)hue, (int)(hslSaturation * 100), (int)(lightness * 100));
        break;
    case 3: // HSV
        swprintf_s(colorText, L"hsv(%d, %d%%, %d%%)", (int)hue, (int)(hsvSaturation * 100), (int)(value * 100));
        break;
    default:
        swprintf_s(colorText, L"#%02X%02X%02X", red, green, blue);
        break;
    }

    
    // 复制到剪贴板
    if (OpenClipboard(m_hwnd))
    {
        EmptyClipboard();
        
        size_t len = wcslen(colorText);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(WCHAR));
        if (hMem)
        {
            WCHAR* pMem = (WCHAR*)GlobalLock(hMem);
            wcscpy_s(pMem, len + 1, colorText);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
        
        CloseClipboard();
        
        MessageBoxW(m_hwnd, colorText, L"颜色已复制", MB_ICONINFORMATION);
    }
}

/**
 * 切换当前颜色信息显示模式。
 */
void ColorPicker::SwitchDisplayMode()
{
    m_displayMode = (m_displayMode + 1) % 3;
}

void ColorPicker::DrawOverlay(HDC hdc)
{

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    
    // 绘制屏幕截图
    if (m_pScreenBitmap)
    {
        graphics.DrawImage(m_pScreenBitmap, 0, 0);
    }
    
    // 绘制放大镜
    DrawMagnifier(hdc, m_currentPoint);
    
    // 绘制颜色信息
    DrawColorInfo(hdc, m_currentPoint);
}

void ColorPicker::DrawMagnifier(HDC hdc, POINT pt)
{
    if (!m_pScreenBitmap)
        return;
    
    Graphics graphics(hdc);
    
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    
    int srcX = pt.x - screenX - MAGNIFIER_ZOOM / 2;
    int srcY = pt.y - screenY - MAGNIFIER_ZOOM / 2;
    
    // 放大镜位置（鼠标右下方）
    int magX = pt.x - screenX + 20;
    int magY = pt.y - screenY + 20;
    
    // 绘制放大镜背景
    SolidBrush bgBrush(Color(255, 50, 50, 50));
    graphics.FillRectangle(&bgBrush, magX, magY, MAGNIFIER_SIZE, MAGNIFIER_SIZE);
    
    // 绘制放大内容
    graphics.DrawImage(m_pScreenBitmap, 
                      Rect(magX, magY, MAGNIFIER_SIZE, MAGNIFIER_SIZE),
                      srcX, srcY, MAGNIFIER_ZOOM, MAGNIFIER_ZOOM,
                      UnitPixel);
    
    // 绘制中心十字线
    Pen crossPen(Color(255, 255, 0, 0), 1);
    int centerX = magX + MAGNIFIER_SIZE / 2;
    int centerY = magY + MAGNIFIER_SIZE / 2;
    graphics.DrawLine(&crossPen, centerX, magY, centerX, magY + MAGNIFIER_SIZE);
    graphics.DrawLine(&crossPen, magX, centerY, magX + MAGNIFIER_SIZE, centerY);
    
    // 绘制边框
    Pen borderPen(Color(255, 255, 255, 255), 2);
    graphics.DrawRectangle(&borderPen, magX, magY, MAGNIFIER_SIZE, MAGNIFIER_SIZE);
}

void ColorPicker::DrawColorInfo(HDC hdc, POINT pt)
{
    Graphics graphics(hdc);

    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);

    int infoX = pt.x - screenX + 20;
    int infoY = pt.y - screenY + 180;

    // 绘制信息背景
    SolidBrush bgBrush(Color(220, 0, 0, 0));
    graphics.FillRectangle(&bgBrush, infoX, infoY, 240, 135);

    // 绘制颜色块
    SolidBrush colorBrush(m_currentColor);
    graphics.FillRectangle(&colorBrush, infoX + 10, infoY + 10, 50, 50);

    Pen borderPen(Color(255, 255, 255, 255), 1);
    graphics.DrawRectangle(&borderPen, infoX + 10, infoY + 10, 50, 50);

    // 绘制颜色值
    Font font(L"Consolas", 10);
    SolidBrush textBrush(Color(255, 255, 255, 255));

    BYTE red = m_currentColor.GetR();
    BYTE green = m_currentColor.GetG();
    BYTE blue = m_currentColor.GetB();
    float r = red / 255.0f;
    float g = green / 255.0f;
    float b = blue / 255.0f;
    float maxVal = (std::max)((std::max)(r, g), b);
    float minVal = (std::min)((std::min)(r, g), b);
    float delta = maxVal - minVal;
    float hue = 0.0f;
    float hsvSaturation = maxVal == 0.0f ? 0.0f : delta / maxVal;
    float value = maxVal;
    float lightness = (maxVal + minVal) / 2.0f;
    float hslSaturation = 0.0f;

    if (delta > 0.0f)
    {
        hslSaturation = delta / (1.0f - fabsf(2.0f * lightness - 1.0f));
        if (maxVal == r)
            hue = 60.0f * (((g - b) / delta) + (g < b ? 6.0f : 0.0f));
        else if (maxVal == g)
            hue = 60.0f * (((b - r) / delta) + 2.0f);
        else
            hue = 60.0f * (((r - g) / delta) + 4.0f);
    }

    WCHAR text[256];
    swprintf_s(text, L"HEX: #%02X%02X%02X", red, green, blue);
    graphics.DrawString(text, -1, &font, PointF((REAL)infoX + 70, (REAL)infoY + 10), &textBrush);

    const WCHAR* modeName = L"RGB";
    if (m_displayMode == 1)
    {
        modeName = L"HSV";
        swprintf_s(text, L"HSV: %d, %d%%, %d%%", (int)hue, (int)(hsvSaturation * 100), (int)(value * 100));
    }
    else if (m_displayMode == 2)
    {
        modeName = L"HSL";
        swprintf_s(text, L"HSL: %d, %d%%, %d%%", (int)hue, (int)(hslSaturation * 100), (int)(lightness * 100));
    }
    else
    {
        swprintf_s(text, L"RGB: %d, %d, %d", red, green, blue);
    }
    graphics.DrawString(text, -1, &font, PointF((REAL)infoX + 70, (REAL)infoY + 32), &textBrush);

    swprintf_s(text, L"模式: %s", modeName);
    graphics.DrawString(text, -1, &font, PointF((REAL)infoX + 70, (REAL)infoY + 54), &textBrush);

    // 提示信息
    Font smallFont(L"Microsoft YaHei", 8);
    graphics.DrawString(L"点击复制 HEX，R/H 切换模式", -1, &smallFont, PointF((REAL)infoX + 10, (REAL)infoY + 75), &textBrush);
    graphics.DrawString(L"1复制HEX 2复制RGB 3复制HSL 4复制HSV", -1, &smallFont, PointF((REAL)infoX + 10, (REAL)infoY + 92), &textBrush);
    graphics.DrawString(L"Esc退出", -1, &smallFont, PointF((REAL)infoX + 10, (REAL)infoY + 109), &textBrush);
}


LRESULT CALLBACK ColorPicker::PickerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    
    ColorPicker* pThis = (ColorPicker*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
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
            pThis->DrawOverlay(hdcMem);
        }
        
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
        
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        
        EndPaint(hwnd, &ps);
        return 0;
    }
    
    case WM_MOUSEMOVE:
    {
        if (pThis)
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam) + GetSystemMetrics(SM_XVIRTUALSCREEN);
            pt.y = GET_Y_LPARAM(lParam) + GetSystemMetrics(SM_YVIRTUALSCREEN);
            pThis->UpdateColor(pt);
        }
        return 0;
    }
    
    case WM_LBUTTONDOWN:
    {
        if (pThis)
        {
            pThis->CopyColorToClipboard(0);  // HEX
        }
        return 0;
    }
    
    case WM_KEYDOWN:
    {
        if (wParam == VK_ESCAPE)
        {
            if (pThis)
            {
                pThis->Cancel();
            }
        }
        else if (wParam == 'R' || wParam == 'H')
        {
            if (pThis)
            {
                pThis->SwitchDisplayMode();
                InvalidateRect(hwnd, NULL, FALSE);
            }

        }
        else if (wParam == '1')
        {
            if (pThis)
            {
                pThis->CopyColorToClipboard(0);  // HEX
            }
        }
        else if (wParam == '2')
        {
            if (pThis)
            {
                pThis->CopyColorToClipboard(1);  // RGB
            }
        }
        else if (wParam == '3')
        {
            if (pThis)
            {
                pThis->CopyColorToClipboard(2);  // HSL
            }
        }
        else if (wParam == '4')
        {
            if (pThis)
            {
                pThis->CopyColorToClipboard(3);  // HSV
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
