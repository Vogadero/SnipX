// 取色器

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class SnipXApp;

/**
 * 全屏取色器：捕获屏幕、显示放大镜和颜色信息，支持复制当前颜色。
 */
class ColorPicker
{
public:
    /**
     * 构造取色器。
     *
     * @param pApp 所属应用实例。
     */
    ColorPicker(SnipXApp* pApp);
    ~ColorPicker();

    /**
     * 启动取色流程：捕获当前屏幕并创建全屏遮罩。
     */
    void Start();

    /**
     * 取消取色并销毁遮罩窗口。
     */
    void Cancel();

private:
    static LRESULT CALLBACK PickerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void CreatePickerWindow();
    void DestroyPickerWindow();

    /**
     * 根据屏幕坐标更新当前采样颜色和光标位置。
     *
     * @param pt 屏幕坐标。
     */
    void UpdateColor(POINT pt);

    /**
     * 将当前颜色按指定格式复制到剪贴板。
     *
     * @param format 显示/复制格式索引，与 m_displayMode 一致。
     */
    void CopyColorToClipboard(int format);

    void DrawOverlay(HDC hdc);
    void DrawMagnifier(HDC hdc, POINT pt);
    void DrawColorInfo(HDC hdc, POINT pt);

    /**
     * 切换取色器的颜色显示模式，用于在 RGB/HSV/HSL 之间循环展示完整颜色数据。
     */
    void SwitchDisplayMode();

    SnipXApp* m_pApp;
    HWND m_hwnd;
    bool m_picking;

    // 屏幕截图
    Bitmap* m_pScreenBitmap;

    // 当前颜色
    Color m_currentColor;
    POINT m_currentPoint;

    // 显示模式：0=RGB, 1=HSV, 2=HSL
    int m_displayMode;

    // 放大镜
    static const int MAGNIFIER_SIZE = 150;
    static const int MAGNIFIER_ZOOM = 10;
};

#endif // COLORPICKER_H
