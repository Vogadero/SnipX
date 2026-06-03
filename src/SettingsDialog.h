// 设置对话框

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

class SnipXApp;

class SettingsDialog
{
public:
    SettingsDialog(SnipXApp* pApp);
    ~SettingsDialog();
    
    void Show();
    
private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    void CreateDialogWindow();
    void InitControls(HWND hwnd);
    void LoadSettings(HWND hwnd);
    void SaveSettings(HWND hwnd);
    /**
     * 根据自动保存开关即时启用或禁用自动保存路径控件。
     *
     * @param hwnd 设置对话框窗口句柄。
     */
    void UpdateAutoSaveControls(HWND hwnd);

    /**
     * 创建保存设置页和标注设置页的控件。
     *
     * @param hwnd 设置对话框窗口句柄。
     */
    void InitAdvancedControls(HWND hwnd);
    /**
     * 按当前选中的 Tab 显示对应设置页控件。
     */
    void UpdateTabVisibility();
    /**
     * 打开文件夹选择对话框并写入指定编辑框。
     *
     * @param editControlId 目标路径编辑框控件 ID。
     */
    void BrowseFolder(int editControlId);
    /**
     * 打开系统颜色选择器并更新默认标注颜色。
     */
    void ChooseDefaultColor();
    /**
     * 刷新 JPG 质量和标注粗细滑块的数值标签。
     */
    void UpdateSliderLabels();
    /**
     * 将所有子控件设置为默认 GUI 字体。
     *
     * @param hwnd 设置对话框窗口句柄。
     */
    void ApplyDefaultFont(HWND hwnd);
    
    // 控件ID
    enum ControlIDs
    {
        // 基本设置
        IDC_HOTKEY = 1001,
        IDC_STARTUP_TRAY = 1002,
        IDC_STARTUP_CAPTURE = 1003,
        IDC_EXIT_TRAY = 1004,
        IDC_EXIT_QUIT = 1005,
        IDC_AUTO_START = 1006,
        IDC_TRAY_VISIBLE = 1007,
        
        // 保存设置
        IDC_FORMAT_PNG = 1010,
        IDC_FORMAT_JPG = 1011,
        IDC_FORMAT_BMP = 1012,
        IDC_SAVE_PATH = 1013,
        IDC_BROWSE_PATH = 1014,
        IDC_JPG_QUALITY = 1015,
        IDC_JPG_QUALITY_LABEL = 1016,
        IDC_AUTO_SAVE = 1017,
        IDC_AUTO_SAVE_PATH = 1018,
        IDC_BROWSE_AUTO_PATH = 1019,
        IDC_FILE_PREFIX = 1020,
        
        // 标注设置
        IDC_DEFAULT_COLOR = 1030,
        IDC_DEFAULT_WIDTH = 1031,
        IDC_DEFAULT_WIDTH_LABEL = 1032,
        
        // 按钮
        IDC_OK = 1040,
        IDC_CANCEL = 1041,
        IDC_APPLY = 1042,
        
        // Tab控件
        IDC_TAB = 1050
    };
    
    SnipXApp* m_pApp;
    HWND m_hwnd;
    HWND m_hTab;
    
    // 当前Tab页
    int m_currentTab;
    
    // Tab页窗口
    HWND m_hTabPages[3];
};

#endif // SETTINGSDIALOG_H
