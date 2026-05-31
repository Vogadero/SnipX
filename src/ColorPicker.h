// 取色器

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class SnipXApp;

class ColorPicker
{
public:
    ColorPicker(SnipXApp* pApp);
    ~ColorPicker();
    
    void Start();
    void Cancel();
    
private:
    static LRESULT CALLBACK PickerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    void CreatePickerWindow();
    void DestroyPickerWindow();
    void UpdateColor(POINT pt);
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
