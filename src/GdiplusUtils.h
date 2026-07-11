// GDI+/Win32 公共工具

#ifndef GDI_PLUS_UTILS_H
#define GDI_PLUS_UTILS_H

#include <windows.h>
#include <gdiplus.h>
#include <string>

using namespace Gdiplus;

/**
 * 虚拟桌面几何信息（支持多显示器）。
 */
struct VirtualScreenInfo
{
    int x;
    int y;
    int width;
    int height;
};

/**
 * 获取当前虚拟桌面原点与尺寸。
 *
 * @return 虚拟桌面几何信息。
 */
VirtualScreenInfo GetVirtualScreenInfo();

/**
 * 获取 GDI+ 图片编码器 CLSID。
 *
 * @param mimeFormat MIME 格式，例如 image/png。
 * @param pClsid 输出编码器 CLSID。
 * @return 找到编码器时返回 true。
 */
bool GetEncoderClsidByMime(const WCHAR* mimeFormat, CLSID* pClsid);

/**
 * 捕获整个虚拟桌面为 GDI+ 位图。
 *
 * @return 调用方负责 delete 的 Bitmap；失败返回 nullptr。
 */
Bitmap* CaptureVirtualScreenBitmap();

/**
 * 绘制放大镜浮层。
 *
 * @param graphics 目标画布。
 * @param sourceBitmap 屏幕位图（虚拟桌面坐标系对应其像素）。
 * @param screenPt 屏幕坐标中的采样中心点。
 * @param magnifierSize 放大镜边长像素。
 * @param magnifierZoom 采样源区域边长像素。
 */
void DrawMagnifierOverlay(Graphics& graphics,
                          Bitmap* sourceBitmap,
                          POINT screenPt,
                          int magnifierSize,
                          int magnifierZoom);

/**
 * 将位图写入剪贴板。
 *
 * @param hwndOwner 打开剪贴板的窗口。
 * @param hBitmap 位图句柄；成功后所有权交给系统，失败时本函数会 DeleteObject。
 * @return 成功写入时返回 true。
 */
bool SetClipboardBitmap(HWND hwndOwner, HBITMAP hBitmap);

/**
 * 将宽字符串写入剪贴板（CF_UNICODETEXT）。
 *
 * @param hwndOwner 打开剪贴板的窗口。
 * @param text 要复制的文本。
 * @return 成功写入时返回 true。
 */
bool SetClipboardUnicodeText(HWND hwndOwner, const WCHAR* text);

#endif // GDI_PLUS_UTILS_H
