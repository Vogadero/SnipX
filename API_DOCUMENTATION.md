# SnipX API 文档

本文档为开发者提供 SnipX 的核心 API 接口说明，方便扩展和定制功能。

## 目录

- [核心类](#核心类)
  - [SnipXApp](#snipxapp)
  - [Config](#config)
  - [ScreenCapture](#screencapture)
  - [Editor](#editor)
  - [Logger](#logger)
- [扩展点](#扩展点)
- [代码示例](#代码示例)

---

## 核心类

### SnipXApp

主应用程序类，管理所有模块的生命周期。

#### 公共方法

```cpp
class SnipXApp
{
public:
    // 构造函数
    SnipXApp(HINSTANCE hInstance);
    
    // 初始化应用程序
    bool Initialize();
    
    // 启动截图
    void StartCapture();
    
    // 启动全屏截图
    void StartFullScreenCapture();
    
    // 启动上次选区截图
    void StartLastSelectionCapture();
    
    // 启动滚动截图
    void StartScrollingCapture();
    
    // 切换录屏状态
    void ToggleRecording();
    
    // 检查是否正在录屏
    bool IsRecording() const;
    
    // 启动取色器
    void StartColorPicker();
    
    // 创建贴图窗口
    void CreatePinWindow(Bitmap* pBitmap);
    
    // 从剪贴板创建贴图
    bool CreatePinWindowFromClipboard(HWND hwndOwner);
    
    // 显示设置对话框
    void ShowSettings();
    
    // 退出应用程序
    void Exit();
    
    // 获取配置对象
    Config* GetConfig() { return m_pConfig; }
    
    // 获取实例句柄
    HINSTANCE GetInstance() { return m_hInstance; }
    
    // 截图完成回调
    void OnCaptureComplete(Bitmap* pBitmap);
};
```

#### 使用示例

```cpp
// 创建应用程序实例
SnipXApp* app = new SnipXApp(hInstance);
if (app->Initialize())
{
    // 启动截图
    app->StartCapture();
}
```

---

### Config

配置管理类，负责加载、保存和管理应用程序配置。

#### 公共方法

```cpp
class Config
{
public:
    Config();
    
    // 加载配置
    bool Load();
    
    // 保存配置
    bool Save();
    
    // 热键设置
    DWORD GetHotkey() const;
    void SetHotkey(DWORD hotkey);
    
    // 启动模式
    StartupMode GetStartupMode() const;
    void SetStartupMode(StartupMode mode);
    
    // 开机自启
    bool IsAutoStart() const;
    void SetAutoStart(bool enable);
    
    // 托盘图标
    bool IsTrayIconVisible() const;
    void SetTrayIconVisible(bool visible);
    
    // 保存设置
    std::wstring GetSavePath() const;
    void SetSavePath(const std::wstring& path);
    
    ImageFormat GetDefaultFormat() const;
    void SetDefaultFormat(ImageFormat format);
    
    int GetJpegQuality() const;
    void SetJpegQuality(int quality);
    
    std::wstring GetFileNamePrefix() const;
    void SetFileNamePrefix(const std::wstring& prefix);
    
    bool IsAutoSave() const;
    void SetAutoSave(bool enable);
    
    // 标注设置
    COLORREF GetDefaultColor() const;
    void SetDefaultColor(COLORREF color);
    
    int GetDefaultThickness() const;
    void SetDefaultThickness(int thickness);
};
```

#### 配置文件位置

配置文件保存在：`%APPDATA%\SnipX\config.ini`

#### 配置示例

```ini
[General]
Hotkey=0x00005804
StartupMode=0
AutoStart=0
TrayIconVisible=1

[Save]
SavePath=C:\Users\Username\Pictures\SnipX
DefaultFormat=0
JpegQuality=90
FileNamePrefix=SnipX_
AutoSave=0

[Annotation]
DefaultColor=16711680
DefaultThickness=2
```

---

### ScreenCapture

截图引擎类，负责屏幕捕获和选区管理。

#### 公共方法

```cpp
class ScreenCapture
{
public:
    ScreenCapture(SnipXApp* pApp);
    
    // 启动截图（选区模式）
    void Start();
    
    // 全屏截图
    void CaptureFullScreen();
    
    // 上次选区截图
    bool CaptureLastSelection();
    
    // 滚动截图
    bool CaptureScrollingWindow();
    
    // 获取截图位图
    Bitmap* GetCaptureBitmap();
    
    // 获取选区矩形
    RECT GetSelectionRect() const;
};
```

#### 截图流程

1. 调用 `Start()` 启动截图
2. 用户拖拽选择区域
3. 按 Enter 确认或双击完成
4. 触发 `SnipXApp::OnCaptureComplete()` 回调
5. 编辑器打开截图

#### 扩展示例：自定义截图处理

```cpp
// 在 SnipXApp::OnCaptureComplete 中添加自定义处理
void SnipXApp::OnCaptureComplete(Bitmap* pBitmap)
{
    if (!pBitmap) return;
    
    // 自定义处理：添加水印
    Graphics g(pBitmap);
    Font font(L"Arial", 12);
    SolidBrush brush(Color(128, 255, 255, 255));
    g.DrawString(L"SnipX", -1, &font, PointF(10, 10), &brush);
    
    // 继续正常流程
    if (m_pEditor)
    {
        m_pEditor->Open(pBitmap);
    }
}
```

---

### Editor

编辑器类，提供标注工具和图像编辑功能。

#### 公共方法

```cpp
class Editor
{
public:
    Editor(SnipXApp* pApp);
    
    // 打开编辑器
    void Open(Bitmap* pBitmap);
    
    // 关闭编辑器
    void Close();
    
    // 保存图像
    bool Save(const std::wstring& path, ImageFormat format);
    
    // 复制到剪贴板
    bool CopyToClipboard();
    
    // 撤销/重做
    void Undo();
    void Redo();
    
    // 工具选择
    void SetTool(ToolType tool);
    ToolType GetCurrentTool() const;
    
    // 颜色和粗细
    void SetColor(COLORREF color);
    COLORREF GetColor() const;
    
    void SetThickness(int thickness);
    int GetThickness() const;
};
```

#### 工具类型

```cpp
enum ToolType
{
    TOOL_SELECT,      // 选择工具
    TOOL_RECTANGLE,   // 矩形
    TOOL_ELLIPSE,     // 椭圆
    TOOL_ARROW,       // 箭头
    TOOL_LINE,        // 直线
    TOOL_PENCIL,      // 铅笔
    TOOL_TEXT,        // 文字
    TOOL_MOSAIC,      // 马赛克
    TOOL_BLUR,        // 模糊
    TOOL_NUMBER,      // 序号
    TOOL_HIGHLIGHT    // 高亮
};
```

#### 标注对象基类

```cpp
class Annotation
{
public:
    virtual void Draw(Graphics* g) = 0;
    virtual bool HitTest(int x, int y) = 0;
    virtual void Move(int dx, int dy) = 0;
    virtual void Resize(int handle, int x, int y) = 0;
    virtual RECT GetBounds() = 0;
};
```

#### 扩展示例：添加自定义标注工具

```cpp
// 1. 定义新的标注类
class CustomAnnotation : public Annotation
{
private:
    POINT m_start;
    POINT m_end;
    COLORREF m_color;
    
public:
    CustomAnnotation(POINT start, POINT end, COLORREF color)
        : m_start(start), m_end(end), m_color(color) {}
    
    void Draw(Graphics* g) override
    {
        // 自定义绘制逻辑
        Pen pen(Color(GetRValue(m_color), GetGValue(m_color), GetBValue(m_color)), 2);
        g->DrawLine(&pen, m_start.x, m_start.y, m_end.x, m_end.y);
    }
    
    bool HitTest(int x, int y) override
    {
        // 碰撞检测逻辑
        return false;
    }
    
    void Move(int dx, int dy) override
    {
        m_start.x += dx;
        m_start.y += dy;
        m_end.x += dx;
        m_end.y += dy;
    }
    
    void Resize(int handle, int x, int y) override
    {
        // 调整大小逻辑
    }
    
    RECT GetBounds() override
    {
        RECT rc;
        rc.left = min(m_start.x, m_end.x);
        rc.top = min(m_start.y, m_end.y);
        rc.right = max(m_start.x, m_end.x);
        rc.bottom = max(m_start.y, m_end.y);
        return rc;
    }
};

// 2. 在 Editor 中添加工具
// 修改 Editor.h 添加新工具类型
enum ToolType
{
    // ... 现有工具
    TOOL_CUSTOM  // 自定义工具
};

// 3. 在 Editor.cpp 的绘制逻辑中处理
void Editor::OnMouseUp(int x, int y)
{
    if (m_currentTool == TOOL_CUSTOM)
    {
        CustomAnnotation* ann = new CustomAnnotation(m_startPoint, m_endPoint, m_color);
        m_annotations.push_back(ann);
    }
}
```

---

### Logger

日志系统类，提供线程安全的日志记录功能。

#### 公共方法

```cpp
class Logger
{
public:
    static Logger& GetInstance();
    
    // 初始化日志系统
    void Initialize(const std::wstring& logPath);
    
    // 设置日志级别
    void SetLevel(LogLevel level);
    
    // 记录日志
    void Log(LogLevel level, const wchar_t* format, ...);
    
    // 便捷方法
    void Debug(const wchar_t* format, ...);
    void Info(const wchar_t* format, ...);
    void Warning(const wchar_t* format, ...);
    void Error(const wchar_t* format, ...);
    
    // 刷新日志
    void Flush();
};
```

#### 日志级别

```cpp
enum LogLevel
{
    LOG_DEBUG = 0,    // 调试信息
    LOG_INFO = 1,     // 一般信息
    LOG_WARNING = 2,  // 警告
    LOG_ERROR = 3     // 错误
};
```

#### 使用宏

```cpp
// 便捷日志宏
LOG_DEBUG(L"Debug message: %d", value);
LOG_INFO(L"Info message: %s", text);
LOG_WARNING(L"Warning message");
LOG_ERROR(L"Error occurred: %d", errorCode);
```

#### 日志文件位置

日志文件保存在：`%APPDATA%\SnipX\snipx.log`

#### 日志格式

```
[2024-01-15 10:30:45.123] [INFO ] SnipX starting...
[2024-01-15 10:30:45.234] [INFO ] COM initialized
[2024-01-15 10:30:45.345] [INFO ] GDI+ initialized
[2024-01-15 10:30:45.456] [ERROR] Failed to register hotkey
```

---

## 扩展点

### 1. 自定义热键

修改 `HotkeyManager.cpp` 添加新的热键：

```cpp
// 注册额外的热键
bool HotkeyManager::RegisterCustomHotkey(UINT modifiers, UINT vk, int id)
{
    return RegisterHotKey(m_hwnd, id, modifiers, vk);
}

// 在消息处理中响应
LRESULT HotkeyManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_HOTKEY)
    {
        int id = (int)wParam;
        if (id == CUSTOM_HOTKEY_ID)
        {
            // 执行自定义操作
            m_pApp->CustomAction();
        }
    }
}
```

### 2. 自定义保存格式

扩展 `Editor::Save()` 支持新格式：

```cpp
bool Editor::Save(const std::wstring& path, ImageFormat format)
{
    // 现有格式处理
    if (format == ImageFormat::PNG || format == ImageFormat::JPEG)
    {
        // ... 现有代码
    }
    
    // 添加新格式
    if (format == ImageFormat::WEBP)
    {
        // WebP 保存逻辑
        return SaveAsWebP(path);
    }
    
    return false;
}
```

### 3. 插件系统（未来扩展）

预留插件接口：

```cpp
// Plugin.h
class ISnipXPlugin
{
public:
    virtual ~ISnipXPlugin() {}
    
    // 插件信息
    virtual const wchar_t* GetName() = 0;
    virtual const wchar_t* GetVersion() = 0;
    
    // 生命周期
    virtual bool Initialize(SnipXApp* app) = 0;
    virtual void Shutdown() = 0;
    
    // 事件钩子
    virtual void OnCaptureStart() {}
    virtual void OnCaptureComplete(Bitmap* bitmap) {}
    virtual void OnSave(const std::wstring& path) {}
};

// 插件管理器
class PluginManager
{
public:
    void LoadPlugin(const std::wstring& dllPath);
    void UnloadPlugin(const std::wstring& name);
    void NotifyEvent(PluginEvent event, void* data);
};
```

---

## 代码示例

### 示例 1：程序化截图并保存

```cpp
#include "SnipX.h"

void AutoCapture(SnipXApp* app)
{
    // 启动全屏截图
    app->StartFullScreenCapture();
    
    // 等待截图完成（实际应用中使用回调）
    Sleep(100);
    
    // 获取编辑器并保存
    Editor* editor = app->GetEditor();
    if (editor)
    {
        std::wstring path = L"C:\\Screenshots\\auto_capture.png";
        editor->Save(path, ImageFormat::PNG);
        editor->Close();
    }
}
```

### 示例 2：批量添加水印

```cpp
void AddWatermarkBatch(const std::vector<std::wstring>& files)
{
    for (const auto& file : files)
    {
        // 加载图像
        Bitmap* bitmap = Bitmap::FromFile(file.c_str());
        if (!bitmap) continue;
        
        // 添加水印
        Graphics g(bitmap);
        Font font(L"Arial", 16, FontStyleBold);
        SolidBrush brush(Color(128, 255, 255, 255));
        
        PointF pos(bitmap->GetWidth() - 150.0f, bitmap->GetHeight() - 40.0f);
        g.DrawString(L"SnipX", -1, &font, pos, &brush);
        
        // 保存
        CLSID pngClsid;
        GetEncoderClsid(L"image/png", &pngClsid);
        bitmap->Save(file.c_str(), &pngClsid);
        
        delete bitmap;
    }
}
```

### 示例 3：自定义快捷键处理

```cpp
// 在 SnipXApp 中添加自定义操作
void SnipXApp::OnCustomHotkey()
{
    LOG_INFO(L"Custom hotkey triggered");
    
    // 截图并自动上传
    StartFullScreenCapture();
    
    // 在 OnCaptureComplete 中处理上传
    // ... 上传逻辑
}

// 注册自定义热键（Ctrl+Shift+U）
m_pHotkeyMgr->RegisterCustomHotkey(
    MOD_CONTROL | MOD_SHIFT, 
    'U', 
    CUSTOM_HOTKEY_UPLOAD
);
```

### 示例 4：集成 OCR

```cpp
#include "tesseract/baseapi.h"

std::wstring PerformOCR(Bitmap* bitmap)
{
    // 转换为 Tesseract 格式
    tesseract::TessBaseAPI ocr;
    ocr.Init(NULL, "chi_sim");  // 中文简体
    
    // 设置图像
    ocr.SetImage(/* bitmap data */);
    
    // 识别文字
    char* text = ocr.GetUTF8Text();
    std::wstring result = ConvertToWString(text);
    
    delete[] text;
    ocr.End();
    
    return result;
}

// 在编辑器中添加 OCR 按钮
void Editor::OnOCRButtonClick()
{
    std::wstring text = PerformOCR(m_pBitmap);
    MessageBoxW(m_hwnd, text.c_str(), L"OCR 结果", MB_OK);
}
```

---

## 编译和链接

### 添加新模块

1. 创建 `.h` 和 `.cpp` 文件
2. 在 `CMakeLists.txt` 中添加到 `SOURCES` 和 `HEADERS`
3. 在 `build.bat` 中添加编译和链接命令

### 链接外部库

```cmake
# CMakeLists.txt
target_link_libraries(${PROJECT_NAME}
    gdiplus
    shell32
    # 添加新库
    your_library
)
```

---

## 调试技巧

### 启用调试日志

```cpp
// 在 main.cpp 中设置日志级别
Logger::GetInstance().Initialize(L"");
Logger::GetInstance().SetLevel(LOG_DEBUG);
```

### 内存泄漏检测

```cpp
// 使用 Visual Studio 的 CRT 调试功能
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// 在 main 函数末尾
#ifdef _DEBUG
_CrtDumpMemoryLeaks();
#endif
```

### 性能分析

```cpp
// 简单的性能计时器
class Timer
{
    LARGE_INTEGER m_start, m_freq;
public:
    Timer() {
        QueryPerformanceFrequency(&m_freq);
        QueryPerformanceCounter(&m_start);
    }
    
    double Elapsed() {
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        return (double)(end.QuadPart - m_start.QuadPart) / m_freq.QuadPart;
    }
};

// 使用
Timer timer;
// ... 执行操作
LOG_INFO(L"Operation took %.3f seconds", timer.Elapsed());
```

---

## 常见问题

### Q: 如何添加新的标注工具？

A: 参考 [扩展示例：添加自定义标注工具](#扩展示例添加自定义标注工具)

### Q: 如何修改默认热键？

A: 修改 `Config.cpp` 中的默认值或在设置界面中更改

### Q: 如何支持新的图像格式？

A: 扩展 `Editor::Save()` 方法，添加新格式的编码器

### Q: 如何集成云存储上传？

A: 在 `OnCaptureComplete()` 或 `Editor::Save()` 后添加上传逻辑

---

## 贡献指南

欢迎贡献代码！请遵循以下规范：

1. **代码风格**：使用 4 空格缩进，大括号另起一行
2. **命名规范**：
   - 类名：PascalCase（如 `ScreenCapture`）
   - 函数名：PascalCase（如 `StartCapture`）
   - 变量名：camelCase（如 `m_pBitmap`）
   - 成员变量：`m_` 前缀
3. **注释**：使用中文或英文，关键逻辑必须注释
4. **日志**：重要操作添加日志记录
5. **错误处理**：检查返回值，记录错误日志

---

## 许可证

SnipX 采用 MIT 许可证，详见 LICENSE 文件。

---

## 联系方式

- GitHub: https://github.com/vogadero/SnipX
- Issues: https://github.com/vogadero/SnipX/issues
- Email: 请通过 GitHub Issues 或 Discussions 联系维护者

---

**最后更新**: 2024-01-15
