// 贴图窗口

#ifndef PINWINDOW_H
#define PINWINDOW_H

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class SnipXApp;

/**
 * 桌面贴图窗口：把截图或剪贴板图片悬浮显示在桌面顶部。
 *
 * 支持拖拽移动、滚轮缩放、透明度调节、鼠标穿透、复制和保存。
 */
class PinWindow
{
public:
    /**
     * 构造贴图窗口。
     *
     * @param pApp 所属应用实例。
     * @param pBitmap 贴图内容位图；对象会接管该指针生命周期。
     */
    PinWindow(SnipXApp* pApp, Bitmap* pBitmap);
    ~PinWindow();

    /**
     * 创建并显示贴图窗口。
     */
    void Show();

    /**
     * 关闭并销毁贴图窗口。
     */
    void Close();

private:
    static LRESULT CALLBACK PinWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void CreatePinWindow();
    void DestroyPinWindow();
    void DrawPin(HDC hdc);
    void ShowContextMenu();

    /**
     * 设置窗口透明度。
     *
     * @param opacity 0-255，数值越大越不透明。
     */
    void SetOpacity(int opacity);

    /**
     * 启用或关闭鼠标穿透。
     *
     * @param through true 时点击会穿透到下层窗口。
     */
    void SetMouseThrough(bool through);

    void CopyToClipboard(HWND hwndOwner);
    void SaveImage(HWND hwndOwner);

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
