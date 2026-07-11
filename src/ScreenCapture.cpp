// 截图引擎实现

#include "ScreenCapture.h"
#include "SnipX.h"
#include "Config.h"
#include "GdiplusUtils.h"
#include <algorithm>
#include <cstdlib>
#include <dwmapi.h>
#include <shlobj.h>
#include <windowsx.h>
#include <vector>


#pragma comment(lib, "dwmapi.lib")

namespace
{
    /**
     * 生成用于滚动截图文件名的本地时间戳。
     */
    std::wstring CreateScrollingTimestamp()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        WCHAR timestamp[32] = { 0 };
        swprintf_s(timestamp, L"%04d%02d%02d_%02d%02d%02d",
                   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        return timestamp;
    }

    /**
     * 将窗口矩形裁剪到虚拟桌面范围内。
     *
     * @param rect 需要裁剪的窗口矩形。
     * @param outOriginX 可选；输出虚拟桌面原点 X。
     * @param outOriginY 可选；输出虚拟桌面原点 Y。
     * @return 裁剪后的矩形。
     */
    RECT ClipRectToVirtualScreen(const RECT& rect, int* outOriginX = nullptr, int* outOriginY = nullptr)
    {
        RECT clipped = rect;
        VirtualScreenInfo screen = GetVirtualScreenInfo();
        RECT screenRect;
        screenRect.left = screen.x;
        screenRect.top = screen.y;
        screenRect.right = screen.x + screen.width;
        screenRect.bottom = screen.y + screen.height;

        if (clipped.left < screenRect.left) clipped.left = screenRect.left;
        if (clipped.top < screenRect.top) clipped.top = screenRect.top;
        if (clipped.right > screenRect.right) clipped.right = screenRect.right;
        if (clipped.bottom > screenRect.bottom) clipped.bottom = screenRect.bottom;

        if (outOriginX)
            *outOriginX = screenRect.left;
        if (outOriginY)
            *outOriginY = screenRect.top;
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

    // 先截整屏，再枚举窗口，最后建遮罩层进入交互选区
    CaptureScreen();
    DetectWindows();
    CreateCaptureWindow();
}

void ScreenCapture::CaptureFullScreen()
{
    if (m_capturing)
        return;

    VirtualScreenInfo screen = GetVirtualScreenInfo();
    RECT fullScreenRect;
    fullScreenRect.left = screen.x;
    fullScreenRect.top = screen.y;
    fullScreenRect.right = screen.x + screen.width;
    fullScreenRect.bottom = screen.y + screen.height;

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
            const SHORT wheelDelta = static_cast<SHORT>(-WHEEL_DELTA * 5);
            const WPARAM wheelParam = static_cast<WPARAM>(static_cast<WORD>(wheelDelta)) << 16;
            SendMessageW(targetWindow, WM_MOUSEWHEEL, wheelParam, 0);
            Sleep(220);
        }

        CaptureScreen();
        if (!m_pScreenBitmap)
            continue;

        // 窗口矩形是屏幕坐标，需减去虚拟桌面原点才是位图像素坐标
        VirtualScreenInfo screen = GetVirtualScreenInfo();
        int x = windowRect.left - screen.x;
        int y = windowRect.top - screen.y;
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

    if (m_pApp && m_pApp->GetConfig())
    {
        std::wstring outputPath = m_pApp->GetConfig()->GetScrollingCapturePath();
        if (!outputPath.empty())
        {
            CreateDirectoryW(outputPath.c_str(), NULL);
            std::wstring filename = outputPath + L"\\Scrolling_" + CreateScrollingTimestamp() + L".png";
            CLSID pngClsid;
            if (GetEncoderClsidByMime(L"image/png", &pngClsid) && finalBitmap->Save(filename.c_str(), &pngClsid, NULL) == Ok)
            {
                m_pApp->GetConfig()->AddHistoryItem(filename);
                m_pApp->GetConfig()->Save();
                std::wstring savedMessage = L"滚动截图已保存到：\n" + filename + L"\n\n接下来将在编辑器中打开。";
                MessageBoxW(NULL, savedMessage.c_str(), L"SnipX 滚动截图", MB_ICONINFORMATION);
            }
        }
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

    // 选区先裁到虚拟桌面，再换算为屏幕位图局部坐标
    int screenX = 0;
    int screenY = 0;
    RECT clipped = ClipRectToVirtualScreen(selection, &screenX, &screenY);
    int width = clipped.right - clipped.left;
    int height = clipped.bottom - clipped.top;
    if (width <= 0 || height <= 0)
        return;

    int x = clipped.left - screenX;
    int y = clipped.top - screenY;

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
        // 所有权交给编辑器，本地指针随后置空避免双重释放
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
    // 捕获完整虚拟桌面，保证多显示器坐标一致
    if (m_pScreenBitmap)
        delete m_pScreenBitmap;
    m_pScreenBitmap = CaptureVirtualScreenBitmap();
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
    
    // 创建覆盖整个虚拟桌面的遮罩窗口
    VirtualScreenInfo screen = GetVirtualScreenInfo();
    m_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                            L"SnipXCaptureWindow", L"",
                            WS_POPUP,
                            screen.x, screen.y, screen.width, screen.height,
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
        m_selectRect.left = (std::min)(m_startPoint.x, m_currentPoint.x);
        m_selectRect.top = (std::min)(m_startPoint.y, m_currentPoint.y);
        m_selectRect.right = (std::max)(m_startPoint.x, m_currentPoint.x);
        m_selectRect.bottom = (std::max)(m_startPoint.y, m_currentPoint.y);
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

void ScreenCapture::DrawOverlay(HDC hdc)
{
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    
    // 绘制屏幕截图
    if (m_pScreenBitmap)
    {
        graphics.DrawImage(m_pScreenBitmap, 0, 0);
    }
    
    // 绘制半透明遮罩：四向挖空选区，突出当前选择
    SolidBrush maskBrush(Color(128, 0, 0, 0));
    VirtualScreenInfo screen = GetVirtualScreenInfo();
    int screenWidth = screen.width;
    int screenHeight = screen.height;

    if (!IsRectEmpty(&m_selectRect))
    {
        // 绘制选区外的遮罩（四个矩形）
        int x = m_selectRect.left - screen.x;
        int y = m_selectRect.top - screen.y;
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
    // 统一放大镜绘制，保持截图与取色器观感一致
    DrawMagnifierOverlay(graphics, m_pScreenBitmap, pt, MAGNIFIER_SIZE, MAGNIFIER_ZOOM);
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
            // 客户区坐标 + 虚拟桌面原点 → 屏幕坐标
            VirtualScreenInfo screen = GetVirtualScreenInfo();
            pThis->m_startPoint.x = GET_X_LPARAM(lParam) + screen.x;
            pThis->m_startPoint.y = GET_Y_LPARAM(lParam) + screen.y;
            SetCapture(hwnd);
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        if (pThis)
        {
            VirtualScreenInfo screen = GetVirtualScreenInfo();
            POINT pt;
            pt.x = GET_X_LPARAM(lParam) + screen.x;
            pt.y = GET_Y_LPARAM(lParam) + screen.y;
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
        else if (wParam == 'R' || wParam == 'H')
        {
            if (pThis && pThis->m_pApp)
            {
                pThis->Cancel();
                pThis->m_pApp->StartColorPicker();
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
