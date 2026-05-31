// 贴图窗口

#ifndef PINWINDOW_H
#define PINWINDOW_H

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class SnipXApp;

class PinWindow
{
public:
    PinWindow(SnipXApp* pApp, Bitmap* pBitmap);
    ~PinWindow();
    
    void Show();
    void Close();
    
private:
    static LRESULT CALLBACK PinWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    void CreatePinWindow();
    void DestroyPinWindow();
    void DrawPin(HDC hdc);
    void ShowContextMenu();
    void SetOpacity(int opacity);
    void SetMouseThrough(bool through);
    /**
     * 根据滚轮方向缩放贴图窗口，并保持原图宽高比例。
     *
     * @param wheelDelta 鼠标滚轮增量，大于 0 放大，小于 0 缩小。
     */
    void ZoomByWheelDelta(short wheelDelta);
    /**
     * 设置贴图缩放比例，自动限制最小/最大尺寸并刷新窗口。
     *
     * @param scale 目标缩放比例，1.0 表示原始大小。
     */
    void ApplyScale(double scale);
    /**
     * 将数值限制在指定区间内。
     *
     * @param value 待限制的数值。
     * @param minValue 最小值。
     * @param maxValue 最大值。
     * @return 限制后的数值。
     */
    int ClampSize(int value, int minValue, int maxValue) const;
    
    SnipXApp* m_pApp;
    HWND m_hwnd;
    Bitmap* m_pBitmap;
    
    // 窗口状态
    bool m_dragging;
    POINT m_dragStart;
    POINT m_windowPos;
    
    // 显示设置
    int m_opacity;  // 0-255
    bool m_mouseThrough;
    
    // 窗口大小
    int m_width;
    int m_height;
    int m_originalWidth;
    int m_originalHeight;
    double m_scale;
};

#endif // PINWINDOW_H
