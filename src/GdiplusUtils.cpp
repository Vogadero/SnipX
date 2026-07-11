// GDI+/Win32 公共工具实现

#include "GdiplusUtils.h"
#include <cstdlib>
#include <cstring>

VirtualScreenInfo GetVirtualScreenInfo()
{
    VirtualScreenInfo info;
    info.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    info.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    info.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    info.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return info;
}

bool GetEncoderClsidByMime(const WCHAR* mimeFormat, CLSID* pClsid)
{
    if (!mimeFormat || !pClsid)
        return false;

    UINT num = 0;
    UINT size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return false;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
    if (!pImageCodecInfo)
        return false;

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT i = 0; i < num; i++)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, mimeFormat) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return true;
        }
    }

    free(pImageCodecInfo);
    return false;
}

Bitmap* CaptureVirtualScreenBitmap()
{
    VirtualScreenInfo screen = GetVirtualScreenInfo();
    if (screen.width <= 0 || screen.height <= 0)
        return nullptr;

    // BitBlt 从虚拟桌面原点拷贝完整多显示器画面
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen)
        return nullptr;

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screen.width, screen.height);
    if (!hdcMem || !hBitmap)
    {
        if (hBitmap)
            DeleteObject(hBitmap);
        if (hdcMem)
            DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return nullptr;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, screen.width, screen.height, hdcScreen, screen.x, screen.y, SRCCOPY);
    SelectObject(hdcMem, hOldBitmap);

    Bitmap* result = Bitmap::FromHBITMAP(hBitmap, NULL);

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    return result;
}

void DrawMagnifierOverlay(Graphics& graphics,
                          Bitmap* sourceBitmap,
                          POINT screenPt,
                          int magnifierSize,
                          int magnifierZoom)
{
    if (!sourceBitmap || magnifierSize <= 0 || magnifierZoom <= 0)
        return;

    VirtualScreenInfo screen = GetVirtualScreenInfo();

    // 屏幕坐标换算为位图像素坐标后居中采样
    int srcX = screenPt.x - screen.x - magnifierZoom / 2;
    int srcY = screenPt.y - screen.y - magnifierZoom / 2;

    // 放大镜放在指针右下方，避免遮挡当前像素
    int magX = screenPt.x - screen.x + 20;
    int magY = screenPt.y - screen.y + 20;

    SolidBrush bgBrush(Color(255, 50, 50, 50));
    graphics.FillRectangle(&bgBrush, magX, magY, magnifierSize, magnifierSize);

    graphics.DrawImage(sourceBitmap,
                       Rect(magX, magY, magnifierSize, magnifierSize),
                       srcX, srcY, magnifierZoom, magnifierZoom,
                       UnitPixel);

    Pen crossPen(Color(255, 255, 0, 0), 1);
    int centerX = magX + magnifierSize / 2;
    int centerY = magY + magnifierSize / 2;
    graphics.DrawLine(&crossPen, centerX, magY, centerX, magY + magnifierSize);
    graphics.DrawLine(&crossPen, magX, centerY, magX + magnifierSize, centerY);

    Pen borderPen(Color(255, 255, 255, 255), 2);
    graphics.DrawRectangle(&borderPen, magX, magY, magnifierSize, magnifierSize);
}

bool SetClipboardBitmap(HWND hwndOwner, HBITMAP hBitmap)
{
    if (!hBitmap)
        return false;

    if (!OpenClipboard(hwndOwner))
    {
        DeleteObject(hBitmap);
        return false;
    }

    EmptyClipboard();
    // 成功后句柄归系统；失败则本地释放
    if (!SetClipboardData(CF_BITMAP, hBitmap))
    {
        DeleteObject(hBitmap);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

bool SetClipboardUnicodeText(HWND hwndOwner, const WCHAR* text)
{
    if (!text)
        return false;

    if (!OpenClipboard(hwndOwner))
        return false;

    EmptyClipboard();
    size_t bytes = (wcslen(text) + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hMem)
    {
        CloseClipboard();
        return false;
    }

    void* pMem = GlobalLock(hMem);
    if (!pMem)
    {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    memcpy(pMem, text, bytes);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_UNICODETEXT, hMem))
    {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}
