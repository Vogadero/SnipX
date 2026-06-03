// 截图引擎实现

#include "ScreenCapture.h"
#include "SnipX.h"
#include "Config.h"
#include <dwmapi.h>
#include <windowsx.h>
#include <vector>


#pragma comment(lib, "dwmapi.lib")

namespace
{
    /**
     * 将窗口矩形裁剪到虚拟桌面范围内。
     *
     * @param rect 需要裁剪的窗口矩形。
     * @return 裁剪后的矩形。
     */
    RECT ClipRectToVirtualScreen(const RECT& rect)
    {
        RECT clipped = rect;
        RECT screenRect;
        screenRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        screenRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        screenRect.right = screenRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        screenRect.bottom = screenRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

        if (clipped.left < screenRect.left) clipped.left = screenRect.left;
        if (clipped.top < screenRect.top) clipped.top = screenRect.top;
        if (clipped.right > screenRect.right) clipped.right = screenRect.right;
        if (clipped.bottom > screenRect.bottom) clipped.bottom = screenRect.bottom;
        return clipped;
    }
}

ScreenCapture::ScreenCapture(SnipXApp* pApp)

    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_capturing(false)
    , m_selecting(false)
    , m_pScreenBitmap(nullptr)
    , m_pCapturedBitmap(nullptr)
    , m_pCurrentWindow(nullptr)
{
    SetRectEmpty(&m_selectRect);
    m_startPoint = { 0, 0 };
    m_currentPoint = { 0, 0 };
}

ScreenCapture::~ScreenCapture()
{
    if (m_pScreenBitmap) delete m_pScreenBitmap;
    if (m_pCapturedBitmap) delete m_pCapturedBitmap;
}

void ScreenCapture::Start()
{
    if (m_capturing)
        return;
    
    m_capturing = true;
    
    // 捕获屏幕
    CaptureScreen();
    
    // 检测窗口
    DetectWindows();
    
    // 创建覆盖窗口
    CreateCaptureWindow();
}

void ScreenCapture::CaptureFullScreen()
{
    if (m_capturing)
        return;

    RECT fullScreenRect;
    fullScreenRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    fullScreenRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    fullScreenRect.right = fullScreenRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    fullScreenRect.bottom = fullScreenRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    CaptureScreen();
    CompleteSelection(fullScreenRect, false);
}

bool ScreenCapture::CaptureLastSelection()
{
    if (m_capturing || !m_pApp || !m_pApp->GetConfig()->HasLastSelection())
        return false;

    RECT selection = m_pApp->GetConfig()->GetLastSelection();
    CaptureScreen();
    CompleteSelection(selection, false);
    return true;
}

bool ScreenCapture::CaptureScrollingWindow()
{
    if (m_capturing)
        return false;

    HWND targetWindow = GetForegroundWindow();
    if (!targetWindow)
        return false;

    RECT windowRect;
    if (!GetWindowRect(targetWindow, &windowRect))
        return false;

    windowRect = ClipRectToVirtualScreen(windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    if (width <= 0 || height <= 0)
        return false;

    const int captureCount = 3;
    const int overlap = 80;
    int stride = height - overlap;
    std::vector<Bitmap*> segments;

    for (int i = 0; i < captureCount; i++)
    {
        if (i > 0)
        {
            SendMessageW(targetWindow, WM_MOUSEWHEEL, MAKEWPARAM(0, -WHEEL_DELTA * 5), 0);
            Sleep(220);
        }

        CaptureScreen();
        int x = windowRect.left - GetSystemMetrics(SM_XVIRTUALSCREEN);
        int y = windowRect.top - GetSystemMetrics(SM_YVIRTUALSCREEN);
        Bitmap* segment = m_pScreenBitmap->Clone(x, y, width, height, PixelFormat32bppARGB);
        if (segment)
        {
            segments.push_back(segment);
        }
    }

    if (segments.empty())
        return false;

    int finalHeight = height + (int)(segments.size() - 1) * stride;
    Bitmap* finalBitmap = new Bitmap(width, finalHeight, PixelFormat32bppARGB);
    Graphics graphics(finalBitmap);
    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    graphics.Clear(Color(255, 255, 255, 255));

    for (size_t i = 0; i < segments.size(); i++)
    {
        int destY = (int)i * stride;
        graphics.DrawImage(segments[i], 0, destY, width, height);
        delete segments[i];
    }

    if (m_pApp)
    {
        m_pApp->OnCaptureComplete(finalBitmap);
    }
    else
    {
        delete finalBitmap;
    }
    return true;
}

void ScreenCapture::CompleteSelection(const RECT& selection, bool rememberSelection)
{
    if (IsRectEmpty(&selection))
        return;

    RECT clipped = selection;
    RECT screenRect;
    screenRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    screenRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    screenRect.right = screenRect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    screenRect.bottom = screenRect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (clipped.left < screenRect.left) clipped.left = screenRect.left;
    if (clipped.top < screenRect.top) clipped.top = screenRect.top;
    if (clipped.right > screenRect.right) clipped.right = screenRect.right;
    if (clipped.bottom > screenRect.bottom) clipped.bottom = screenRect.bottom;

    int width = clipped.right - clipped.left;
    int height = clipped.bottom - clipped.top;
    if (width <= 0 || height <= 0)
        return;

    int x = clipped.left - screenRect.left;
    int y = clipped.top - screenRect.top;

    if (m_pCapturedBitmap)
        delete m_pCapturedBitmap;

    m_pCapturedBitmap = m_pScreenBitmap->Clone(x, y, width, height, PixelFormat32bppARGB);

    if (rememberSelection && m_pApp && m_pApp->GetConfig())
    {
        Config* cfg = m_pApp->GetConfig();
        cfg->SetLastSelection(clipped);
        cfg->Save();
    }


    Cancel();

    if (m_pApp && m_pCapturedBitmap)
    {
        m_pApp->OnCaptureComplete(m_pCapturedBitmap);
        m_pCapturedBitmap = nullptr;
    }
}

void ScreenCapture::Cancel()


{
    m_capturing = false;
    DestroyCaptureWindow();
}

void ScreenCapture::CaptureScreen()
{
    // 获取屏幕尺寸（支持多显示器）
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    // 创建屏幕DC
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    
    // 复制屏幕内容
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, screenX, screenY, SRCCOPY);
    
    // 转换为GDI+ Bitmap
    if (m_pScreenBitmap)
        delete m_pScreenBitmap;
    m_pScreenBitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
    
    // 清理
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

void ScreenCapture::DetectWindows()
{
    m_windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)this);
}

BOOL CALLBACK ScreenCapture::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    ScreenCapture* pThis = (ScreenCapture*)lParam;
    
    // 过滤不可见窗口
    if (!IsWindowVisible(hwnd))
        return TRUE;
    
    // 获取窗口矩形（包含DWM阴影）
    RECT rect;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT))))
    {
        WindowInfo info;
        info.hwnd = hwnd;
        info.rect = rect;
        info.level = 0;  // 简化处理，不计算层级
        pThis->m_windows.push_back(info);
    }
    
    return TRUE;
}

WindowInfo* ScreenCapture::FindWindowAtPoint(POINT pt)
{
    // 从后往前查找（后面的窗口在上层）
    for (int i = (int)m_windows.size() - 1; i >= 0; i--)
    {
        if (PtInRect(&m_windows[i].rect, pt))
        {
            return &m_windows[i];
        }
    }
    return nullptr;
}

void ScreenCapture::CreateCaptureWindow()
{
    // 注册窗口类
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = CaptureWndProc;
    wc.hInstance = m_pApp->GetInstance();
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.lpszClassName = L"SnipXCaptureWindow";
    RegisterClassExW(&wc);
    
    // 创建全屏覆盖窗口
    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    m_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                            L"SnipXCaptureWindow", L"",
                            WS_POPUP,
                            screenX, screenY, screenWidth, screenHeight,
                            NULL, NULL, m_pApp->GetInstance(), this);
    
    // 设置窗口透明度
    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void ScreenCapture::DestroyCaptureWindow()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

void ScreenCapture::UpdateSelection(POINT pt)
{
    m_currentPoint = pt;
    
    if (m_selecting)
    {
        // 正在拖拽选区
        m_selectRect.left = min(m_startPoint.x, m_currentPoint.x);
        m_selectRect.top = min(m_startPoint.y, m_currentPoint.y);
        m_selectRect.right = max(m_startPoint.x, m_currentPoint.x);
        m_selectRect.bottom = max(m_startPoint.y, m_currentPoint.y);
    }
    else
    {
        // 检测窗口
        m_pCurrentWindow = FindWindowAtPoint(pt);
        if (m_pCurrentWindow)
        {
            m_selectRect = m_pCurrentWindow->rect;
        }
    }
    
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void ScreenCapture::ConfirmSelection()
{
    if (IsRectEmpty(&m_selectRect))
        return;
    
    // 从屏幕截图中裁剪选区
    int x = m_selectRect.left - GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = m_selectRect.top - GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = m_selectRect.right - m_selectRect.left;
    int height = m_selectRect.bottom - m_selectRect.top;
    
    if (m_pCapturedBitmap)
        delete m_pCapturedBitmap;
    
    m_pCapturedBitmap = m_pScreenBitmap->Clone(x, y, width, height, PixelFormat32bppARGB);
    
    // 关闭捕获窗口
    Cancel();
    
    // 打开编辑器
    if (m_pApp && m_pCapturedBitmap)
    {
        m_pApp->OnCaptureComplete(m_pCapturedBitmap);
        m_pCapturedBitmap = nullptr;  // 转移所有权
    }
}

void ScreenCapture::DrawOverlay(HDC hdc)
{
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    
    // 绘制屏幕截图
    if (m_pScreenBitmap)
    {
        graphics.DrawImage(m_pScreenBitmap, 0, 0);
    }
    
    // 绘制半透明遮罩
    SolidBrush maskBrush(Color(128, 0, 0, 0));
    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    if (!IsRectEmpty(&m_selectRect))
    {
        // 绘制选区外的遮罩（四个矩形）
        int x = m_selectRect.left - GetSystemMetrics(SM_XVIRTUALSCREEN);
        int y = m_selectRect.top - GetSystemMetrics(SM_YVIRTUALSCREEN);
        int w = m_selectRect.right - m_selectRect.left;
        int h = m_selectRect.bottom - m_selectRect.top;
        
        // 上
        graphics.FillRectangle(&maskBrush, 0, 0, screenWidth, y);
        // 下
        graphics.FillRectangle(&maskBrush, 0, y + h, screenWidth, screenHeight - y - h);
        // 左
        graphics.FillRectangle(&maskBrush, 0, y, x, h);
        // 右
        graphics.FillRectangle(&maskBrush, x + w, y, screenWidth - x - w, h);
        
        // 绘制选区边框
        Pen borderPen(Color(255, 0, 120, 215), 2);
        graphics.DrawRectangle(&borderPen, x, y, w, h);
        
        // 绘制尺寸信息
        WCHAR sizeText[64];
        swprintf_s(sizeText, L"%d x %d", w, h);
        
        Font font(L"Microsoft YaHei", 12);
        SolidBrush textBrush(Color(255, 255, 255, 255));
        SolidBrush bgBrush(Color(200, 0, 0, 0));
        
        RectF textRect;
        graphics.MeasureString(sizeText, -1, &font, PointF(0, 0), &textRect);
        
        float textX = (float)(x + w / 2 - textRect.Width / 2);
        float textY = (float)(y + h + 10);
        
        graphics.FillRectangle(&bgBrush, textX - 5, textY - 2, textRect.Width + 10, textRect.Height + 4);
        graphics.DrawString(sizeText, -1, &font, PointF(textX, textY), &textBrush);
    }
    else
    {
        // 全屏遮罩
        graphics.FillRectangle(&maskBrush, 0, 0, screenWidth, screenHeight);
    }
    
    // 绘制放大镜
    if (!m_selecting)
    {
        DrawMagnifier(hdc, m_currentPoint);
    }
}

void ScreenCapture::DrawMagnifier(HDC hdc, POINT pt)
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
    
    // 绘制十字线
    Pen crossPen(Color(255, 255, 0, 0), 1);
    int centerX = magX + MAGNIFIER_SIZE / 2;
    int centerY = magY + MAGNIFIER_SIZE / 2;
    graphics.DrawLine(&crossPen, centerX, magY, centerX, magY + MAGNIFIER_SIZE);
    graphics.DrawLine(&crossPen, magX, centerY, magX + MAGNIFIER_SIZE, centerY);
    
    // 绘制边框
    Pen borderPen(Color(255, 255, 255, 255), 2);
    graphics.DrawRectangle(&borderPen, magX, magY, MAGNIFIER_SIZE, MAGNIFIER_SIZE);
}

LRESULT CALLBACK ScreenCapture::CaptureWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    
    ScreenCapture* pThis = (ScreenCapture*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        // 双缓冲绘制
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
    
    case WM_LBUTTONDOWN:
    {
        if (pThis)
        {
            pThis->m_selecting = true;
            pThis->m_startPoint.x = GET_X_LPARAM(lParam) + GetSystemMetrics(SM_XVIRTUALSCREEN);
            pThis->m_startPoint.y = GET_Y_LPARAM(lParam) + GetSystemMetrics(SM_YVIRTUALSCREEN);
            SetCapture(hwnd);
        }
        return 0;
    }
    
    case WM_MOUSEMOVE:
    {
        if (pThis)
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam) + GetSystemMetrics(SM_XVIRTUALSCREEN);
            pt.y = GET_Y_LPARAM(lParam) + GetSystemMetrics(SM_YVIRTUALSCREEN);
            pThis->UpdateSelection(pt);
        }
        return 0;
    }
    
    case WM_LBUTTONUP:
    {
        if (pThis && pThis->m_selecting)
        {
            pThis->m_selecting = false;
            ReleaseCapture();
            RECT selection = pThis->m_selectRect;
            pThis->CompleteSelection(selection, true);
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
        else if (wParam == VK_RETURN)
        {
            if (pThis && !IsRectEmpty(&pThis->m_selectRect))
            {
                RECT selection = pThis->m_selectRect;
                pThis->CompleteSelection(selection, true);
            }
        }

        else if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN)
        {
            if (pThis && !IsRectEmpty(&pThis->m_selectRect))
            {
                // 方向键微调选区
                int dx = 0, dy = 0;
                if (wParam == VK_LEFT) dx = -1;
                else if (wParam == VK_RIGHT) dx = 1;
                else if (wParam == VK_UP) dy = -1;
                else if (wParam == VK_DOWN) dy = 1;
                
                // 如果按住Shift，移动10像素
                if (GetKeyState(VK_SHIFT) < 0)
                {
                    dx *= 10;
                    dy *= 10;
                }
                
                pThis->m_selectRect.left += dx;
                pThis->m_selectRect.right += dx;
                pThis->m_selectRect.top += dy;
                pThis->m_selectRect.bottom += dy;
                
                InvalidateRect(hwnd, NULL, FALSE);
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
