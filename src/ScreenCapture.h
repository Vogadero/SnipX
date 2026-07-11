// 截图引擎

#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <windows.h>
#include <gdiplus.h>
#include <vector>

using namespace Gdiplus;

class SnipXApp;

/**
 * 窗口命中检测用的轻量窗口信息。
 */
struct WindowInfo
{
    HWND hwnd;
    RECT rect;
    int level;  // 层级深度，值越大通常表示更靠前
};

/**
 * 区域/全屏/上次选区/滚动截图引擎。
 *
 * 负责全屏遮罩、选区拖拽、窗口检测、放大镜辅助，并把裁剪结果交给编辑器。
 */
class ScreenCapture
{
public:
    /**
     * 构造截图引擎。
     *
     * @param pApp 所属应用实例。
     */
    ScreenCapture(SnipXApp* pApp);
    ~ScreenCapture();

    /**
     * 进入交互式区域截图：捕获屏幕、创建遮罩窗口并等待用户确认选区。
     */
    void Start();

    /**
     * 直接捕获虚拟桌面全屏并进入编辑器。
     */
    void CaptureFullScreen();

    /**
     * 使用配置中保存的上次选区直接截图。
     *
     * @return 成功复用上次选区并进入编辑器时返回 true。
     */
    bool CaptureLastSelection();

    /**
     * 对当前前台窗口执行简化滚动截图。
     *
     * @return 成功生成滚动截图并进入编辑器时返回 true。
     */
    bool CaptureScrollingWindow();

    /**
     * 取消当前交互截图并销毁遮罩窗口。
     */
    void Cancel();

    /**
     * 获取最近一次完成的截图位图。
     *
     * @return 已完成截图的位图指针；未完成时可能为空。
     */
    Bitmap* GetCapturedBitmap() { return m_pCapturedBitmap; }

private:
    static LRESULT CALLBACK CaptureWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    void CreateCaptureWindow();
    void DestroyCaptureWindow();
    void CaptureScreen();
    void DetectWindows();
    WindowInfo* FindWindowAtPoint(POINT pt);

    /**
     * 按虚拟桌面坐标裁剪指定选区并转交编辑器。
     *
     * @param selection 虚拟桌面坐标中的截图区域。
     * @param rememberSelection 是否保存为上次选区。
     */
    void CompleteSelection(const RECT& selection, bool rememberSelection);
    void UpdateSelection(POINT pt);
    void DrawOverlay(HDC hdc);
    void DrawMagnifier(HDC hdc, POINT pt);

    SnipXApp* m_pApp;
    HWND m_hwnd;
    bool m_capturing;
    bool m_selecting;

    // 屏幕截图
    Bitmap* m_pScreenBitmap;
    Bitmap* m_pCapturedBitmap;

    // 选区
    RECT m_selectRect;
    POINT m_startPoint;
    POINT m_currentPoint;

    // 窗口检测
    std::vector<WindowInfo> m_windows;
    WindowInfo* m_pCurrentWindow;

    // 放大镜
    static const int MAGNIFIER_SIZE = 120;
    static const int MAGNIFIER_ZOOM = 8;
};

#endif // SCREENCAPTURE_H
