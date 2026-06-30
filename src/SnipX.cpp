// SnipX 主应用类实现

#include "SnipX.h"
#include "Logger.h"

namespace
{
    /**
     * 将剪贴板中的 HBITMAP 克隆为 GDI+ Bitmap。
     *
     * @param hBitmap 剪贴板位图句柄。
     * @return 调用方负责释放的 Bitmap，失败时返回 nullptr。
     */
    Bitmap* CloneClipboardBitmap(HBITMAP hBitmap)
    {
        if (!hBitmap)
            return nullptr;

        Bitmap sourceBitmap(hBitmap, NULL);
        if (sourceBitmap.GetLastStatus() != Ok)
            return nullptr;

        return sourceBitmap.Clone(0, 0, sourceBitmap.GetWidth(), sourceBitmap.GetHeight(), PixelFormat32bppARGB);
    }
}


SnipXApp::SnipXApp(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_gdiplusToken(0)
    , m_pConfig(nullptr)
    , m_pHotkeyMgr(nullptr)
    , m_pTrayIcon(nullptr)
    , m_pCapture(nullptr)
    , m_pRecorder(nullptr)
    , m_pEditor(nullptr)
    , m_pColorPicker(nullptr)
{
}

SnipXApp::~SnipXApp()
{
    // 清理贴图窗口
    for (auto pin : m_pinWindows)
    {
        delete pin;
    }
    m_pinWindows.clear();
    
    if (m_pColorPicker) delete m_pColorPicker;
    if (m_pEditor) delete m_pEditor;
    if (m_pRecorder) delete m_pRecorder;
    if (m_pCapture) delete m_pCapture;
    if (m_pTrayIcon) delete m_pTrayIcon;
    if (m_pHotkeyMgr) delete m_pHotkeyMgr;
    if (m_pConfig) delete m_pConfig;
    
    ShutdownGDIPlus();
}

bool SnipXApp::Initialize()
{
    LOG_INFO(L"Initializing SnipXApp...");
    
    // 初始化GDI+
    if (!InitializeGDIPlus())
    {
        LOG_ERROR(L"Failed to initialize GDI+");
        return false;
    }
    LOG_INFO(L"GDI+ initialized");
    
    // 加载配置
    m_pConfig = new Config();
    m_pConfig->Load();
    LOG_INFO(L"Configuration loaded");
    
    // 初始化热键管理器
    m_pHotkeyMgr = new HotkeyManager(this);
    if (!m_pHotkeyMgr->RegisterHotkey(m_pConfig->GetHotkey()))
    {
        LOG_ERROR(L"Failed to register hotkey");
        MessageBoxW(NULL, L"注册热键失败！", L"SnipX", MB_ICONERROR);
        return false;
    }
    LOG_INFO(L"Hotkey registered successfully");
    
    // 初始化托盘图标
    ApplyTrayIconVisibility();
    
    // 初始化截图引擎

    m_pCapture = new ScreenCapture(this);
    LOG_INFO(L"ScreenCapture initialized");

    // 初始化录屏器
    m_pRecorder = new ScreenRecorder(this);
    LOG_INFO(L"ScreenRecorder initialized");
    
    // 初始化编辑器
    m_pEditor = new Editor(this);
    LOG_INFO(L"Editor initialized");
    
    // 初始化取色器
    m_pColorPicker = nullptr;  // 延迟创建
    
    // 如果配置为启动即截图
    if (m_pConfig->GetStartupMode() == STARTUP_CAPTURE)
    {
        LOG_INFO(L"Starting capture on startup");
        StartCapture();
    }
    
    return true;
}

bool SnipXApp::InitializeGDIPlus()
{
    GdiplusStartupInput gdiplusStartupInput;
    Status status = GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    return (status == Ok);
}

void SnipXApp::ShutdownGDIPlus()
{
    if (m_gdiplusToken)
    {
        GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}

void SnipXApp::StartCapture()
{
    LOG_INFO(L"Starting screen capture");
    if (m_pCapture)
    {
        m_pCapture->Start();
    }
}


void SnipXApp::StartFullScreenCapture()
{
    if (m_pCapture)
    {
        m_pCapture->CaptureFullScreen();
    }
}

void SnipXApp::StartLastSelectionCapture()
{
    if (m_pCapture && !m_pCapture->CaptureLastSelection())
    {
        m_pCapture->Start();
    }
}

void SnipXApp::StartScrollingCapture()
{
    if (!m_pCapture)
        return;

    std::wstring outputPath = m_pConfig ? m_pConfig->GetScrollingCapturePath() : L"";
    std::wstring message = L"将对当前前台窗口执行简化滚动截图。\n\n请先激活需要截图的窗口，并保持窗口内容可滚动。";
    if (!outputPath.empty())
    {
        CreateDirectoryW(outputPath.c_str(), NULL);
        message += L"\n\n默认滚动截图目录：\n" + outputPath;
    }
    message += L"\n\n是否开始？";

    if (MessageBoxW(NULL, message.c_str(), L"SnipX 滚动截图", MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)
        return;

    if (!m_pCapture->CaptureScrollingWindow())
    {
        MessageBoxW(NULL, L"滚动截图失败，请先激活需要截图的窗口，并确认内容可滚动。", L"SnipX", MB_ICONINFORMATION);
    }
}

void SnipXApp::ToggleRecording()
{
    if (!m_pRecorder)
        return;

    if (m_pRecorder->IsRecording())
    {
        std::wstring outputDirectory = m_pRecorder->GetOutputDirectory();
        m_pRecorder->Stop();
        std::wstring gifPath = m_pRecorder->GetGifPath();
        std::wstring message = L"录屏已停止，帧序列已保存到：\n" + outputDirectory;
        if (!gifPath.empty())
        {
            message += L"\n\nGIF 已生成：\n" + gifPath;
        }
        MessageBoxW(NULL, message.c_str(), L"SnipX", MB_ICONINFORMATION);
        return;
    }

    std::wstring outputPath = m_pConfig ? m_pConfig->GetRecordingPath() : L"";
    std::wstring message = L"将开始录制虚拟桌面画面，并保存为 PNG 帧序列。\n\n再次点击托盘菜单中的“停止录屏”即可结束。";
    if (!outputPath.empty())
    {
        CreateDirectoryW(outputPath.c_str(), NULL);
        message += L"\n\n录屏保存目录：\n" + outputPath;
    }
    message += L"\n\n是否开始录屏？";

    if (MessageBoxW(NULL, message.c_str(), L"SnipX 录屏", MB_ICONINFORMATION | MB_OKCANCEL) != IDOK)
        return;

    if (!m_pRecorder->Start())
    {
        MessageBoxW(NULL, L"启动录屏失败，请检查图片目录是否可写。", L"SnipX", MB_ICONERROR);
    }
}

bool SnipXApp::IsRecording() const
{
    return m_pRecorder && m_pRecorder->IsRecording();
}


void SnipXApp::ApplyTrayIconVisibility()


{
    if (!m_pConfig)
        return;

    if (m_pConfig->IsTrayIconVisible())
    {
        if (!m_pTrayIcon)
        {
            m_pTrayIcon = new TrayIcon(this);
        }
        m_pTrayIcon->Show();
    }
    else if (m_pTrayIcon)
    {
        m_pTrayIcon->Hide();
    }
}

void SnipXApp::OnCaptureComplete(Bitmap* pBitmap)

{
    LOG_INFO(L"Capture completed, opening editor");
    if (m_pEditor && pBitmap)
    {
        m_pEditor->Open(pBitmap);
    }
    else
    {
        LOG_ERROR(L"Editor or bitmap is null");
    }
}

void SnipXApp::ShowSettings()
{
    SettingsDialog* dlg = new SettingsDialog(this);
    dlg->Show();
    delete dlg;
}

void SnipXApp::Exit()
{
    PostQuitMessage(0);
}

void SnipXApp::StartColorPicker()
{
    if (!m_pColorPicker)
    {
        m_pColorPicker = new ColorPicker(this);
    }
    m_pColorPicker->Start();
}

void SnipXApp::CreatePinWindow(Bitmap* pBitmap)
{
    if (!pBitmap)
        return;
    
    PinWindow* pin = new PinWindow(this, pBitmap);
    m_pinWindows.push_back(pin);
    pin->Show();
}

bool SnipXApp::CreatePinWindowFromClipboard(HWND hwndOwner)
{
    if (!IsClipboardFormatAvailable(CF_BITMAP))
    {
        MessageBoxW(hwndOwner, L"剪贴板中没有可贴图的位图内容。", L"SnipX", MB_ICONINFORMATION);
        return false;
    }

    if (!OpenClipboard(hwndOwner))
    {
        MessageBoxW(hwndOwner, L"无法打开剪贴板。", L"SnipX", MB_ICONERROR);
        return false;
    }

    HBITMAP hClipboardBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    Bitmap* pinBitmap = CloneClipboardBitmap(hClipboardBitmap);
    CloseClipboard();

    if (!pinBitmap)
    {
        MessageBoxW(hwndOwner, L"读取剪贴板图片失败。", L"SnipX", MB_ICONERROR);
        return false;
    }

    CreatePinWindow(pinBitmap);
    return true;
}

