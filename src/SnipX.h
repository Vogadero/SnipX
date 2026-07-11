// SnipX 主应用类头文件

#ifndef SNIPX_H
#define SNIPX_H

#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include "Config.h"
#include "HotkeyManager.h"
#include "TrayIcon.h"
#include "ScreenCapture.h"
#include "ScreenRecorder.h"
#include "Editor.h"
#include "ColorPicker.h"
#include "PinWindow.h"
#include "SettingsDialog.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

/**
 * SnipX 应用生命周期与功能模块的协调入口。
 *
 * 负责初始化 GDI+/配置/热键/托盘，并把截图、录屏、取色、贴图和设置请求分发给对应子系统。
 */
class SnipXApp
{
public:
    /**
     * 构造应用对象。
     *
     * @param hInstance 当前进程实例句柄。
     */
    SnipXApp(HINSTANCE hInstance);
    ~SnipXApp();

    /**
     * 初始化 GDI+、配置、热键、托盘和各功能模块。
     *
     * @return 初始化成功时返回 true；热键注册失败等关键错误会返回 false。
     * @note 若启动模式为 STARTUP_CAPTURE，会在初始化末尾立即进入截图。
     */
    bool Initialize();

    /**
     * 开始区域截图流程。
     */
    void StartCapture();

    /**
     * 直接截取虚拟桌面全屏并打开编辑器。
     */
    void StartFullScreenCapture();

    /**
     * 使用上次保存的选区截图；没有上次选区时退回普通截图。
     */
    void StartLastSelectionCapture();

    /**
     * 对当前前台窗口执行简化滚动截图。
     *
     * @note 开始前会弹出确认提示，并尝试创建配置中的滚动截图目录。
     */
    void StartScrollingCapture();

    /**
     * 切换录屏状态：未录制时开始，录制中则停止。
     *
     * @note 停止时会提示帧序列目录，以及可选生成的 GIF 路径。
     */
    void ToggleRecording();

    /**
     * 返回当前是否正在录屏。
     */
    bool IsRecording() const;

    /**
     * 截图完成后把位图交给编辑器打开。
     *
     * @param pBitmap 截图结果位图；所有权通常由调用方转入编辑器流程。
     */
    void OnCaptureComplete(Bitmap* pBitmap);

    /**
     * 启动取色器；首次调用时会延迟创建 ColorPicker 实例。
     */
    void StartColorPicker();

    /**
     * 使用给定位图创建贴图窗口并显示。
     *
     * @param pBitmap 贴图内容；空指针会被忽略。
     */
    void CreatePinWindow(Bitmap* pBitmap);

    /**
     * 根据当前配置立即显示或隐藏托盘图标。
     */
    void ApplyTrayIconVisibility();

    /**
     * 从剪贴板读取位图并创建贴图窗口。
     *
     * @param hwndOwner 消息提示所属窗口。
     * @return 成功创建贴图窗口时返回 true。
     */
    bool CreatePinWindowFromClipboard(HWND hwndOwner);

    /**
     * 打开设置对话框；对话框以模态方式运行并在返回后销毁。
     */
    void ShowSettings();

    /**
     * 请求退出主消息循环。
     */
    void Exit();

    HINSTANCE GetInstance() const { return m_hInstance; }
    Config* GetConfig() { return m_pConfig; }

private:
    HINSTANCE m_hInstance;
    ULONG_PTR m_gdiplusToken;

    Config* m_pConfig;
    HotkeyManager* m_pHotkeyMgr;
    TrayIcon* m_pTrayIcon;
    ScreenCapture* m_pCapture;
    ScreenRecorder* m_pRecorder;
    Editor* m_pEditor;
    ColorPicker* m_pColorPicker;
    std::vector<PinWindow*> m_pinWindows;

    /**
     * 启动 GDI+ 并记录 token。
     *
     * @return 启动成功时返回 true。
     */
    bool InitializeGDIPlus();

    /**
     * 按 token 关闭 GDI+。
     */
    void ShutdownGDIPlus();
};

#endif // SNIPX_H
