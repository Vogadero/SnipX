# SnipX 架构设计文档

## 项目概述

SnipX 是一款使用纯 C++ 和 Win32 API 开发的极致轻量级截图工具，目标体积 < 1MB。

## 技术栈

- **语言**：C++ 17
- **UI 框架**：Win32 API（原生）
- **图形库**：GDI+（Windows 自带）
- **编译器**：MSVC / MinGW-w64
- **构建系统**：CMake / 批处理脚本

## 架构设计

### 整体架构

```
┌─────────────────────────────────────────────────────┐
│                   SnipXApp                          │
│              (主应用程序类)                           │
│  - 初始化 GDI+                                       │
│  - 管理各个子模块                                     │
│  - 处理应用程序生命周期                               │
└─────────────────────────────────────────────────────┘
           │
           ├──────────────┬──────────────┬──────────────┬──────────────┐
           │              │              │              │              │
           ▼              ▼              ▼              ▼              ▼
    ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐
    │  Config  │   │ Hotkey   │   │ TrayIcon │   │  Screen  │   │  Editor  │
    │          │   │ Manager  │   │          │   │ Capture  │   │          │
    │配置管理   │   │热键管理   │   │托盘图标   │   │截图引擎   │   │编辑器    │
    └──────────┘   └──────────┘   └──────────┘   └──────────┘   └──────────┘
```

### 模块说明

#### 1. SnipXApp（主应用类）

**职责：**
- 应用程序初始化和清理
- GDI+ 初始化和关闭
- 管理各个子模块的生命周期
- 提供模块间通信接口

**关键方法：**
```cpp
bool Initialize();           // 初始化应用
void StartCapture();         // 启动截图
void ShowSettings();         // 显示设置
void Exit();                 // 退出应用
```

#### 2. Config（配置管理）

**职责：**
- 加载和保存配置文件
- 提供配置项访问接口
- 管理开机自启注册表

**配置存储：**
- 位置：`%APPDATA%\SnipX\config.ini`
- 格式：Windows INI 格式
- 使用 API：`GetPrivateProfileString` / `WritePrivateProfileString`

**配置项：**
```ini
[Hotkey]
Key=0x5801  ; Alt+X

[Save]
DefaultFormat=png
DefaultPath=%USERPROFILE%\Pictures\Screenshots
JpgQuality=90
AutoSave=0
FileNamePrefix=截图

[Appearance]
TrayIconVisible=1
StartupMode=0
ExitMode=0

[Annotation]
DefaultColor=16711680  ; RGB(255,0,0)
DefaultStrokeWidth=2

[Startup]
AutoStart=0
```

#### 3. HotkeyManager（热键管理）

**职责：**
- 注册全局热键
- 监听热键消息
- 触发截图回调

**实现方式：**
- 创建隐藏消息窗口（HWND_MESSAGE）
- 使用 `RegisterHotKey` API
- 在窗口过程中处理 `WM_HOTKEY` 消息

**热键格式：**
```cpp
UINT hotkey = MAKEWORD(VK_X, HOTKEYF_ALT);
// 低字节：虚拟键码
// 高字节：修饰键标志
```

#### 4. TrayIcon（托盘图标）

**职责：**
- 显示系统托盘图标
- 处理托盘图标事件（单击、双击、右键）
- 显示右键菜单

**实现方式：**
- 使用 `Shell_NotifyIcon` API
- 创建隐藏消息窗口接收托盘消息
- 使用 `TrackPopupMenu` 显示菜单

**托盘消息：**
```cpp
#define WM_TRAYICON (WM_USER + 2)

// 消息处理
case WM_LBUTTONDBLCLK:  // 双击截图
case WM_RBUTTONUP:      // 右键菜单
```

#### 5. ScreenCapture（截图引擎）

**职责：**
- 捕获屏幕内容
- 检测窗口边界
- 提供选区交互
- 绘制覆盖层和放大镜

**核心流程：**
```
1. 捕获屏幕 (BitBlt)
   ↓
2. 检测窗口 (EnumWindows + DwmGetWindowAttribute)
   ↓
3. 创建全屏覆盖窗口
   ↓
4. 用户交互（鼠标拖拽选区）
   ↓
5. 确认选区，裁剪图片
   ↓
6. 传递给编辑器
```

**关键技术：**

1. **屏幕捕获**
```cpp
HDC hdcScreen = GetDC(NULL);
HDC hdcMem = CreateCompatibleDC(hdcScreen);
HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
BitBlt(hdcMem, 0, 0, width, height, hdcScreen, x, y, SRCCOPY);
Bitmap* pBitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
```

2. **窗口检测**
```cpp
EnumWindows(EnumWindowsProc, lParam);
DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
```

3. **覆盖窗口**
```cpp
CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
               className, L"", WS_POPUP,
               x, y, width, height, ...);
SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
```

4. **放大镜绘制**
```cpp
graphics.DrawImage(screenBitmap,
                  Rect(magX, magY, MAGNIFIER_SIZE, MAGNIFIER_SIZE),
                  srcX, srcY, MAGNIFIER_ZOOM, MAGNIFIER_ZOOM,
                  UnitPixel);
```

#### 6. Editor（编辑器）

**职责：**
- 显示截图预览
- 提供标注工具
- 管理标注对象
- 保存和导出图片

**标注对象架构：**
```cpp
class Annotation {
    virtual void Draw(Graphics* graphics) = 0;
    virtual bool HitTest(Point pt) = 0;
    
    AnnotationType type;
    Color color;
    int strokeWidth;
    bool selected;
};

class RectangleAnnotation : public Annotation { ... };
class EllipseAnnotation : public Annotation { ... };
class ArrowAnnotation : public Annotation { ... };
class PencilAnnotation : public Annotation { ... };
```

**编辑器布局：**
```
┌────────────────────────────────────┐
│  工具栏（60px）                      │
│  [矩形] [椭圆] [箭头] [铅笔] ...    │
│  [颜色] [粗细]                      │
├────────────────────────────────────┤
│                                    │
│  图片预览区域                        │
│  （绘制截图 + 标注）                  │
│                                    │
├────────────────────────────────────┤
│  底部操作栏（50px）                  │
│  [保存] [复制] [贴图] [关闭]        │
└────────────────────────────────────┘
```

**标注绘制流程：**
```cpp
void Editor::DrawEditor(HDC hdc) {
    Graphics graphics(hdc);
    
    // 1. 绘制背景
    graphics.FillRectangle(&bgBrush, ...);
    
    // 2. 绘制截图
    graphics.DrawImage(m_pBitmap, x, y);
    
    // 3. 绘制所有标注
    for (auto anno : m_annotations) {
        anno->Draw(&graphics);
    }
    
    // 4. 绘制工具栏和按钮
    DrawToolbar(&graphics);
    DrawBottomBar(&graphics);
}
```

## 数据流

### 截图流程

```
用户按下 Alt+X
    ↓
HotkeyManager 接收 WM_HOTKEY
    ↓
调用 SnipXApp::StartCapture()
    ↓
ScreenCapture::Start()
    ├─ CaptureScreen()        // 捕获屏幕
    ├─ DetectWindows()        // 检测窗口
    └─ CreateCaptureWindow()  // 创建覆盖窗口
    ↓
用户拖拽选区
    ↓
ScreenCapture::ConfirmSelection()
    ├─ 裁剪图片
    └─ 传递给 Editor
    ↓
Editor::Open(bitmap)
    ↓
用户编辑标注
    ↓
用户保存/复制
    ↓
Editor::Close()
```

### 配置流程

```
应用启动
    ↓
Config::Load()
    ├─ 读取 config.ini
    └─ 应用配置到各模块
    ↓
用户修改设置
    ↓
Config::Save()
    └─ 写入 config.ini
```

## 内存管理

### 原则
- 使用 RAII 管理资源
- 及时释放 GDI 对象
- 避免内存泄漏

### 关键资源管理

1. **GDI+ 对象**
```cpp
Bitmap* pBitmap = new Bitmap(...);
// 使用
delete pBitmap;  // 必须手动释放
```

2. **GDI 对象**
```cpp
HDC hdc = CreateCompatibleDC(...);
HBITMAP hBitmap = CreateCompatibleBitmap(...);
// 使用
DeleteObject(hBitmap);
DeleteDC(hdc);
```

3. **窗口句柄**
```cpp
HWND hwnd = CreateWindow(...);
// 使用
DestroyWindow(hwnd);
```

## 性能优化

### 1. 双缓冲绘制
```cpp
HDC hdcMem = CreateCompatibleDC(hdc);
HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
SelectObject(hdcMem, hBitmap);

// 在内存 DC 上绘制
DrawContent(hdcMem);

// 一次性复制到屏幕
BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

DeleteObject(hBitmap);
DeleteDC(hdcMem);
```

### 2. 减少重绘
- 使用 `InvalidateRect` 指定刷新区域
- 避免全窗口刷新

### 3. 优化图像处理
- 使用 GDI+ 硬件加速
- 避免频繁的格式转换

## 编译优化

### MSVC 优化选项
```
/O2      - 最大速度优化
/GL      - 全程序优化
/LTCG    - 链接时代码生成
/OPT:REF - 移除未引用函数
/OPT:ICF - 合并相同函数
/MD      - 动态链接运行时（体积小）
```

### MinGW 优化选项
```
-O3      - 最高级别优化
-s       - 移除符号表
-static  - 静态链接
```

## 错误处理

### 策略
- 关键操作检查返回值
- 使用 MessageBox 提示用户
- 记录错误日志（未来）

### 示例
```cpp
if (!RegisterHotKey(...)) {
    MessageBox(NULL, L"注册热键失败！", L"错误", MB_ICONERROR);
    return false;
}
```

## 扩展性设计

### 插件化标注工具
- 使用抽象基类 `Annotation`
- 新增工具只需继承并实现 `Draw` 和 `HitTest`

### 配置扩展
- INI 格式易于手动编辑
- 可轻松添加新配置项

### 多语言支持（未来）
- 字符串资源化
- 使用资源文件或 JSON

## 测试策略

### 单元测试
- 配置读写测试
- 图像处理测试

### 集成测试
- 完整截图流程测试
- 多显示器测试

### 兼容性测试
- Windows 7/8/10/11
- 不同 DPI 设置
- 不同分辨率

## 未来优化方向

1. **代码优化**
   - 减少 STL 使用（改用 C 数组）
   - 内联小函数
   - 使用 C 风格字符串

2. **体积优化**
   - 使用 UPX 压缩
   - 移除未使用的代码
   - 优化资源文件

3. **性能优化**
   - 多线程截图
   - GPU 加速绘制
   - 缓存优化

4. **功能扩展**
   - 插件系统
   - 脚本支持
   - 云同步
