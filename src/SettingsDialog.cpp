// 设置对话框实现

#include "SettingsDialog.h"
#include "SnipX.h"
#include <shlobj.h>

namespace
{
    const int TAB_BASIC = 0;
    const int TAB_SAVE = 1;
    const int TAB_ANNOTATION = 2;

    /**
     * 根据控件所在 Tab 切换可见性。
     *
     * @param hwndParent 父窗口句柄。
     * @param controlId 控件 ID。
     * @param visible 是否显示。
     */
    void SetControlVisible(HWND hwndParent, int controlId, bool visible)
    {
        HWND hwndControl = GetDlgItem(hwndParent, controlId);
        if (hwndControl)
        {
            ShowWindow(hwndControl, visible ? SW_SHOW : SW_HIDE);
        }
    }
}


SettingsDialog::SettingsDialog(SnipXApp* pApp)
    : m_pApp(pApp)
    , m_hwnd(NULL)
    , m_hTab(NULL)
    , m_currentTab(0)
{
    for (int i = 0; i < 3; i++)
    {
        m_hTabPages[i] = NULL;
    }
}

SettingsDialog::~SettingsDialog()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}

void SettingsDialog::Show()
{
    CreateDialogWindow();
}

void SettingsDialog::CreateDialogWindow()
{
    // 已经打开时直接前置，避免重复创建多个设置窗口。
    if (m_hwnd && IsWindow(m_hwnd))
    {
        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
        return;
    }

    // 注册对话框类
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = DefDlgProcW;
    wc.hInstance = m_pApp->GetInstance();
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"SnipXSettingsDialog";
    RegisterClassExW(&wc);
    
    // 创建对话框窗口
    m_hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        L"SnipXSettingsDialog",
        L"SnipX 设置",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 560, 470,
        NULL, NULL, m_pApp->GetInstance(), NULL
    );
    
    if (!m_hwnd)
        return;
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    
    // 初始化控件
    InitControls(m_hwnd);
    
    // 加载设置
    LoadSettings(m_hwnd);
    
    // 居中显示
    RECT rc;
    GetWindowRect(m_hwnd, &rc);
    int x = (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;
    SetWindowPos(m_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    
    // 显示设置窗口，并在窗口关闭前只处理本窗口消息；关闭时不退出主程序。
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);

    MSG msg;
    while (m_hwnd && IsWindow(m_hwnd) && GetMessage(&msg, NULL, 0, 0))
    {
        if (!IsDialogMessage(m_hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void SettingsDialog::InitControls(HWND hwnd)
{
    // 初始化通用控件
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES | ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&icex);
    
    // 创建Tab控件
    m_hTab = CreateWindowExW(
        0, WC_TABCONTROLW, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        10, 10, 530, 380,
        hwnd, (HMENU)IDC_TAB, m_pApp->GetInstance(), NULL
    );
    
    // 添加Tab页
    TCITEMW tie;
    tie.mask = TCIF_TEXT;
    
    tie.pszText = (LPWSTR)L"基本设置";
    TabCtrl_InsertItem(m_hTab, 0, &tie);
    
    tie.pszText = (LPWSTR)L"保存设置";
    TabCtrl_InsertItem(m_hTab, 1, &tie);
    
    tie.pszText = (LPWSTR)L"标注设置";
    TabCtrl_InsertItem(m_hTab, 2, &tie);
    
    // 基本设置页
    CreateWindowW(L"STATIC", L"启动模式：", WS_CHILD | WS_VISIBLE,
                 30, 60, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"驻留托盘", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                 130, 60, 100, 20, hwnd, (HMENU)IDC_STARTUP_TRAY, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"启动即截图", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                 240, 60, 100, 20, hwnd, (HMENU)IDC_STARTUP_CAPTURE, m_pApp->GetInstance(), NULL);
    
    CreateWindowW(L"BUTTON", L"开机自启", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                 30, 100, 100, 20, hwnd, (HMENU)IDC_AUTO_START, m_pApp->GetInstance(), NULL);
    
    CreateWindowW(L"BUTTON", L"显示托盘图标", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                 30, 130, 120, 20, hwnd, (HMENU)IDC_TRAY_VISIBLE, m_pApp->GetInstance(), NULL);

    InitAdvancedControls(hwnd);
    
    // 创建按钮
    CreateWindowW(L"BUTTON", L"确定", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                 260, 405, 80, 30, hwnd, (HMENU)IDC_OK, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"取消", WS_CHILD | WS_VISIBLE,
                 350, 405, 80, 30, hwnd, (HMENU)IDC_CANCEL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"应用", WS_CHILD | WS_VISIBLE,
                 440, 405, 80, 30, hwnd, (HMENU)IDC_APPLY, m_pApp->GetInstance(), NULL);
    
    ApplyDefaultFont(hwnd);
    UpdateTabVisibility();
    
    // 设置窗口过程
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)DialogProc);
}

void SettingsDialog::InitAdvancedControls(HWND hwnd)
{
    CreateWindowW(L"STATIC", L"默认格式：", WS_CHILD | WS_VISIBLE,
                 30, 60, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"PNG", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                 130, 60, 70, 20, hwnd, (HMENU)IDC_FORMAT_PNG, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"JPG", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                 210, 60, 70, 20, hwnd, (HMENU)IDC_FORMAT_JPG, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"BMP", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                 290, 60, 70, 20, hwnd, (HMENU)IDC_FORMAT_BMP, m_pApp->GetInstance(), NULL);

    CreateWindowW(L"STATIC", L"默认路径：", WS_CHILD | WS_VISIBLE,
                 30, 95, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                 130, 92, 240, 24, hwnd, (HMENU)IDC_SAVE_PATH, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"浏览...", WS_CHILD | WS_VISIBLE,
                 380, 92, 70, 24, hwnd, (HMENU)IDC_BROWSE_PATH, m_pApp->GetInstance(), NULL);

    CreateWindowW(L"STATIC", L"JPG质量：", WS_CHILD | WS_VISIBLE,
                 30, 130, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                 130, 125, 210, 30, hwnd, (HMENU)IDC_JPG_QUALITY, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                 350, 130, 80, 20, hwnd, (HMENU)IDC_JPG_QUALITY_LABEL, m_pApp->GetInstance(), NULL);

    CreateWindowW(L"STATIC", L"文件前缀：", WS_CHILD | WS_VISIBLE,
                 30, 165, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                 130, 162, 240, 24, hwnd, (HMENU)IDC_FILE_PREFIX, m_pApp->GetInstance(), NULL);

    CreateWindowW(L"BUTTON", L"启用自动保存", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                 30, 200, 140, 20, hwnd, (HMENU)IDC_AUTO_SAVE, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"STATIC", L"自动保存路径：", WS_CHILD | WS_VISIBLE,
                 30, 235, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                 130, 232, 300, 24, hwnd, (HMENU)IDC_AUTO_SAVE_PATH, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"浏览...", WS_CHILD | WS_VISIBLE,
                 440, 232, 70, 24, hwnd, (HMENU)IDC_BROWSE_AUTO_PATH, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"STATIC", L"录屏路径：", WS_CHILD | WS_VISIBLE,
                 30, 270, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                 130, 267, 300, 24, hwnd, (HMENU)IDC_RECORDING_PATH, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"浏览...", WS_CHILD | WS_VISIBLE,
                 440, 267, 70, 24, hwnd, (HMENU)IDC_BROWSE_RECORDING_PATH, m_pApp->GetInstance(), NULL);

    CreateWindowW(L"STATIC", L"滚动截图路径：", WS_CHILD | WS_VISIBLE,
                 30, 305, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                 130, 302, 300, 24, hwnd, (HMENU)IDC_SCROLLING_PATH, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"浏览...", WS_CHILD | WS_VISIBLE,
                 440, 302, 70, 24, hwnd, (HMENU)IDC_BROWSE_SCROLLING_PATH, m_pApp->GetInstance(), NULL);

    SendDlgItemMessage(hwnd, IDC_JPG_QUALITY, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
    SendDlgItemMessage(hwnd, IDC_JPG_QUALITY, TBM_SETTICFREQ, 10, 0);

    CreateWindowW(L"STATIC", L"默认颜色：", WS_CHILD | WS_VISIBLE,
                 30, 60, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"BUTTON", L"选择颜色...", WS_CHILD | WS_VISIBLE,
                 130, 56, 100, 28, hwnd, (HMENU)IDC_DEFAULT_COLOR, m_pApp->GetInstance(), NULL);

    CreateWindowW(L"STATIC", L"默认粗细：", WS_CHILD | WS_VISIBLE,
                 30, 105, 100, 20, hwnd, NULL, m_pApp->GetInstance(), NULL);
    CreateWindowW(TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                 130, 100, 210, 30, hwnd, (HMENU)IDC_DEFAULT_WIDTH, m_pApp->GetInstance(), NULL);
    CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                 350, 105, 80, 20, hwnd, (HMENU)IDC_DEFAULT_WIDTH_LABEL, m_pApp->GetInstance(), NULL);

    SendDlgItemMessage(hwnd, IDC_DEFAULT_WIDTH, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
    SendDlgItemMessage(hwnd, IDC_DEFAULT_WIDTH, TBM_SETTICFREQ, 1, 0);
}

void SettingsDialog::LoadSettings(HWND hwnd)
{
    Config* cfg = m_pApp->GetConfig();
    
    // 加载启动模式
    if (cfg->GetStartupMode() == STARTUP_TRAY)
        CheckRadioButton(hwnd, IDC_STARTUP_TRAY, IDC_STARTUP_CAPTURE, IDC_STARTUP_TRAY);
    else
        CheckRadioButton(hwnd, IDC_STARTUP_TRAY, IDC_STARTUP_CAPTURE, IDC_STARTUP_CAPTURE);
    
    // 加载开机自启
    CheckDlgButton(hwnd, IDC_AUTO_START, cfg->IsAutoStart() ? BST_CHECKED : BST_UNCHECKED);
    
    // 加载托盘图标显示
    CheckDlgButton(hwnd, IDC_TRAY_VISIBLE, cfg->IsTrayIconVisible() ? BST_CHECKED : BST_UNCHECKED);

    std::wstring format = cfg->GetDefaultFormat();
    int formatControl = IDC_FORMAT_PNG;
    if (format == L"jpg" || format == L"jpeg")
        formatControl = IDC_FORMAT_JPG;
    else if (format == L"bmp")
        formatControl = IDC_FORMAT_BMP;
    CheckRadioButton(hwnd, IDC_FORMAT_PNG, IDC_FORMAT_BMP, formatControl);

    SetDlgItemTextW(hwnd, IDC_SAVE_PATH, cfg->GetDefaultPath().c_str());
    SetDlgItemTextW(hwnd, IDC_AUTO_SAVE_PATH, cfg->GetAutoSavePath().c_str());
    SetDlgItemTextW(hwnd, IDC_RECORDING_PATH, cfg->GetRecordingPath().c_str());
    SetDlgItemTextW(hwnd, IDC_SCROLLING_PATH, cfg->GetScrollingCapturePath().c_str());
    SetDlgItemTextW(hwnd, IDC_FILE_PREFIX, cfg->GetFileNamePrefix().c_str());
    CheckDlgButton(hwnd, IDC_AUTO_SAVE, cfg->IsAutoSave() ? BST_CHECKED : BST_UNCHECKED);
    UpdateAutoSaveControls(hwnd);
    SendDlgItemMessage(hwnd, IDC_JPG_QUALITY, TBM_SETPOS, TRUE, cfg->GetJpgQuality());

    SendDlgItemMessage(hwnd, IDC_DEFAULT_WIDTH, TBM_SETPOS, TRUE, cfg->GetDefaultStrokeWidth());
    UpdateSliderLabels();
}

/**
 * 根据自动保存选项刷新路径输入和浏览按钮状态。
 */
void SettingsDialog::UpdateAutoSaveControls(HWND hwnd)
{
    BOOL enabled = IsDlgButtonChecked(hwnd, IDC_AUTO_SAVE) == BST_CHECKED;
    EnableWindow(GetDlgItem(hwnd, IDC_AUTO_SAVE_PATH), enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_BROWSE_AUTO_PATH), enabled);
}

void SettingsDialog::SaveSettings(HWND hwnd)
{

    Config* cfg = m_pApp->GetConfig();
    
    // 保存启动模式
    if (IsDlgButtonChecked(hwnd, IDC_STARTUP_TRAY) == BST_CHECKED)
    {
        cfg->SetStartupMode(STARTUP_TRAY);
    }
    else
    {
        cfg->SetStartupMode(STARTUP_CAPTURE);
    }
    
    // 保存开机自启
    bool autoStart = IsDlgButtonChecked(hwnd, IDC_AUTO_START) == BST_CHECKED;
    cfg->SetAutoStart(autoStart);
    
    // 保存托盘图标显示
    cfg->SetTrayIconVisible(IsDlgButtonChecked(hwnd, IDC_TRAY_VISIBLE) == BST_CHECKED);
    m_pApp->ApplyTrayIconVisibility();

    if (IsDlgButtonChecked(hwnd, IDC_FORMAT_JPG) == BST_CHECKED)

        cfg->SetDefaultFormat(L"jpg");
    else if (IsDlgButtonChecked(hwnd, IDC_FORMAT_BMP) == BST_CHECKED)
        cfg->SetDefaultFormat(L"bmp");
    else
        cfg->SetDefaultFormat(L"png");

    WCHAR textBuffer[MAX_PATH] = { 0 };
    GetDlgItemTextW(hwnd, IDC_SAVE_PATH, textBuffer, MAX_PATH);
    cfg->SetDefaultPath(textBuffer);

    GetDlgItemTextW(hwnd, IDC_AUTO_SAVE_PATH, textBuffer, MAX_PATH);
    if (textBuffer[0] == L'\0')
    {
        GetDlgItemTextW(hwnd, IDC_SAVE_PATH, textBuffer, MAX_PATH);
    }
    cfg->SetAutoSavePath(textBuffer);


    GetDlgItemTextW(hwnd, IDC_FILE_PREFIX, textBuffer, MAX_PATH);
    cfg->SetFileNamePrefix(textBuffer);

    GetDlgItemTextW(hwnd, IDC_RECORDING_PATH, textBuffer, MAX_PATH);
    cfg->SetRecordingPath(textBuffer);

    GetDlgItemTextW(hwnd, IDC_SCROLLING_PATH, textBuffer, MAX_PATH);
    cfg->SetScrollingCapturePath(textBuffer);

    cfg->SetAutoSave(IsDlgButtonChecked(hwnd, IDC_AUTO_SAVE) == BST_CHECKED);
    cfg->SetJpgQuality((int)SendDlgItemMessage(hwnd, IDC_JPG_QUALITY, TBM_GETPOS, 0, 0));
    cfg->SetDefaultStrokeWidth((int)SendDlgItemMessage(hwnd, IDC_DEFAULT_WIDTH, TBM_GETPOS, 0, 0));
    
    // 保存配置文件
    cfg->Save();
    
    MessageBoxW(hwnd, L"设置已保存！", L"SnipX", MB_ICONINFORMATION);
}

void SettingsDialog::UpdateTabVisibility()
{
    int saveControls[] = {
        IDC_FORMAT_PNG, IDC_FORMAT_JPG, IDC_FORMAT_BMP,
        IDC_SAVE_PATH, IDC_BROWSE_PATH, IDC_JPG_QUALITY, IDC_JPG_QUALITY_LABEL,
        IDC_AUTO_SAVE, IDC_AUTO_SAVE_PATH, IDC_BROWSE_AUTO_PATH, IDC_FILE_PREFIX,
        IDC_RECORDING_PATH, IDC_BROWSE_RECORDING_PATH, IDC_SCROLLING_PATH, IDC_BROWSE_SCROLLING_PATH
    };
    int annotationControls[] = {
        IDC_DEFAULT_COLOR, IDC_DEFAULT_WIDTH, IDC_DEFAULT_WIDTH_LABEL
    };

    for (int controlId : saveControls)
    {
        SetControlVisible(m_hwnd, controlId, m_currentTab == TAB_SAVE);
    }
    for (int controlId : annotationControls)
    {
        SetControlVisible(m_hwnd, controlId, m_currentTab == TAB_ANNOTATION);
    }

    // 静态标签未设置 ID，保留简化布局：非当前页控件隐藏，静态说明文字允许复用显示。
}

void SettingsDialog::BrowseFolder(int editControlId)
{
    BROWSEINFOW bi = { 0 };
    bi.hwndOwner = m_hwnd;
    bi.lpszTitle = L"请选择文件夹";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST itemList = SHBrowseForFolderW(&bi);
    if (!itemList)
        return;

    WCHAR path[MAX_PATH] = { 0 };
    if (SHGetPathFromIDListW(itemList, path))
    {
        SetDlgItemTextW(m_hwnd, editControlId, path);
    }
    CoTaskMemFree(itemList);
}

void SettingsDialog::ChooseDefaultColor()
{
    Config* cfg = m_pApp->GetConfig();
    static COLORREF customColors[16] = { 0 };
    CHOOSECOLORW cc = { 0 };
    cc.lStructSize = sizeof(CHOOSECOLORW);
    cc.hwndOwner = m_hwnd;
    cc.rgbResult = cfg->GetDefaultColor();
    cc.lpCustColors = customColors;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColorW(&cc))
    {
        cfg->SetDefaultColor(cc.rgbResult);
    }
}

void SettingsDialog::UpdateSliderLabels()
{
    WCHAR label[32] = { 0 };
    int jpgQuality = (int)SendDlgItemMessage(m_hwnd, IDC_JPG_QUALITY, TBM_GETPOS, 0, 0);
    swprintf_s(label, L"%d%%", jpgQuality);
    SetDlgItemTextW(m_hwnd, IDC_JPG_QUALITY_LABEL, label);

    int width = (int)SendDlgItemMessage(m_hwnd, IDC_DEFAULT_WIDTH, TBM_GETPOS, 0, 0);
    swprintf_s(label, L"%d px", width);
    SetDlgItemTextW(m_hwnd, IDC_DEFAULT_WIDTH_LABEL, label);
}

void SettingsDialog::ApplyDefaultFont(HWND hwnd)
{
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
        SendMessage(hwndChild, WM_SETFONT, (WPARAM)lParam, TRUE);
        return TRUE;
    }, (LPARAM)hFont);
}

INT_PTR CALLBACK SettingsDialog::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)

{
    SettingsDialog* pThis = (SettingsDialog*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_BROWSE_PATH:
            if (pThis)
            {
                pThis->BrowseFolder(IDC_SAVE_PATH);
            }
            return TRUE;

        case IDC_BROWSE_AUTO_PATH:
            if (pThis)
            {
                pThis->BrowseFolder(IDC_AUTO_SAVE_PATH);
            }
            return TRUE;

        case IDC_BROWSE_RECORDING_PATH:
            if (pThis)
            {
                pThis->BrowseFolder(IDC_RECORDING_PATH);
            }
            return TRUE;

        case IDC_BROWSE_SCROLLING_PATH:
            if (pThis)
            {
                pThis->BrowseFolder(IDC_SCROLLING_PATH);
            }
            return TRUE;

        case IDC_AUTO_SAVE:
            if (pThis)
            {
                pThis->UpdateAutoSaveControls(hwnd);
            }
            return TRUE;

        case IDC_DEFAULT_COLOR:
            if (pThis)
            {
                pThis->ChooseDefaultColor();
            }
            return TRUE;

        case IDC_OK:

            if (pThis)
            {
                pThis->SaveSettings(hwnd);
                pThis->m_hwnd = NULL;
                DestroyWindow(hwnd);
            }
            return TRUE;

        case IDC_CANCEL:
            if (pThis)
                pThis->m_hwnd = NULL;
            DestroyWindow(hwnd);
            return TRUE;
            
        case IDC_APPLY:
            if (pThis)
            {
                pThis->SaveSettings(hwnd);
            }
            return TRUE;
        }
        break;
    }
    case WM_HSCROLL:
        if (pThis)
        {
            pThis->UpdateSliderLabels();
        }
        return TRUE;

    case WM_NOTIFY:
        if (pThis && ((LPNMHDR)lParam)->idFrom == IDC_TAB && ((LPNMHDR)lParam)->code == TCN_SELCHANGE)
        {
            pThis->m_currentTab = TabCtrl_GetCurSel(pThis->m_hTab);
            pThis->UpdateTabVisibility();
        }
        return TRUE;
    
    case WM_CLOSE:
        if (pThis)
            pThis->m_hwnd = NULL;
        DestroyWindow(hwnd);
        return TRUE;

    case WM_DESTROY:
        if (pThis)
            pThis->m_hwnd = NULL;
        return TRUE;
    }
    
    return FALSE;
}
