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

class SnipXApp
{
public:
    SnipXApp(HINSTANCE hInstance);
    ~SnipXApp();
    
    bool Initialize();
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
     */
    void StartScrollingCapture();
    /**
     * 切换录屏状态：未录制时开始，录制中则停止。
     */
    void ToggleRecording();
    /**
     * 返回当前是否正在录屏。
     */
    bool IsRecording() const;
    void OnCaptureComplete(Bitmap* pBitmap);



    void StartColorPicker();
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
    void ShowSettings();
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
    
    bool InitializeGDIPlus();
    void ShutdownGDIPlus();
};

#endif // SNIPX_H
