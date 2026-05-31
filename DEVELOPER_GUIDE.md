# SnipX 开发者指南

欢迎参与 SnipX 的开发！本指南将帮助你快速上手项目开发。

## 目录

- [开发环境搭建](#开发环境搭建)
- [项目结构](#项目结构)
- [编码规范](#编码规范)
- [开发流程](#开发流程)
- [调试技巧](#调试技巧)
- [贡献指南](#贡献指南)

---

## 开发环境搭建

### 必需工具

1. **编译器**（选择其一）：
   - Visual Studio 2019/2022（推荐）
   - MinGW-w64 8.1.0 或更高版本

2. **构建工具**：
   - CMake 3.15 或更高版本（可选）

3. **版本控制**：
   - Git

4. **代码编辑器**（推荐）：
   - Visual Studio Code
   - Visual Studio
   - CLion

### 可选工具

- **UPX**: 用于压缩可执行文件
- **7-Zip**: 用于打包发布
- **Doxygen**: 用于生成文档
- **Valgrind/Dr. Memory**: 用于内存检测

### 环境配置

#### 使用 Visual Studio

1. 安装 Visual Studio 2019/2022
2. 选择"使用 C++ 的桌面开发"工作负载
3. 确保安装了 Windows SDK

#### 使用 MinGW

1. 下载 MinGW-w64：https://www.mingw-w64.org/
2. 添加 MinGW 的 bin 目录到 PATH
3. 验证安装：
   ```cmd
   g++ --version
   ```

### 克隆项目

```bash
git clone https://github.com/yourusername/SnipX.git
cd SnipX
```

### 编译项目

#### 方法 1：使用 build.bat

```cmd
build.bat
```

#### 方法 2：使用 CMake

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 运行程序

```cmd
bin\SnipX.exe
```

---

## 项目结构

```
SnipX/
├── .github/              # GitHub 配置
│   └── workflows/        # CI/CD 工作流
│       └── release.yml   # 自动发布
├── src/                  # 源代码
│   ├── main.cpp          # 程序入口
│   ├── SnipX.cpp/h       # 主应用类
│   ├── Config.cpp/h      # 配置管理
│   ├── HotkeyManager.cpp/h    # 热键管理
│   ├── TrayIcon.cpp/h    # 托盘图标
│   ├── ScreenCapture.cpp/h    # 截图引擎
│   ├── Editor.cpp/h      # 编辑器
│   ├── ColorPicker.cpp/h # 取色器
│   ├── PinWindow.cpp/h   # 贴图窗口
│   ├── SettingsDialog.cpp/h   # 设置对话框
│   ├── ScreenRecorder.cpp/h   # 录屏器
│   ├── Localization.cpp/h     # 多语言
│   ├── Platform.cpp/h    # 跨平台层
│   ├── Logger.cpp/h      # 日志系统
│   ├── Version.h         # 版本信息
│   └── resource.h        # 资源定义
├── res/                  # 资源文件
│   ├── icon.ico          # 应用图标
│   └── SnipX.rc          # 资源脚本
├── bin/                  # 编译输出（自动生成）
├── obj/                  # 中间文件（自动生成）
├── release/              # 发布文件（自动生成）
├── build.bat             # 编译脚本
├── compile_simple.bat    # 简化编译脚本
├── release.bat           # 发布打包脚本
├── CMakeLists.txt        # CMake 配置
├── README.md             # 项目说明
├── COMPILE.md            # 编译说明
├── TODO.md               # 任务清单
├── ARCHITECTURE.md       # 架构文档
├── API_DOCUMENTATION.md  # API 文档
├── USER_MANUAL.md        # 用户手册
├── DEVELOPER_GUIDE.md    # 开发者指南（本文档）
├── PERFORMANCE_OPTIMIZATION.md  # 性能优化
├── CHANGELOG.md          # 更新日志
└── .gitignore            # Git 忽略文件
```

### 核心模块说明

| 模块 | 文件 | 功能 |
|------|------|------|
| 主应用 | SnipX.cpp/h | 应用程序生命周期管理 |
| 配置 | Config.cpp/h | 配置加载、保存、管理 |
| 热键 | HotkeyManager.cpp/h | 全局热键注册和处理 |
| 托盘 | TrayIcon.cpp/h | 系统托盘图标和菜单 |
| 截图 | ScreenCapture.cpp/h | 屏幕捕获和选区管理 |
| 编辑 | Editor.cpp/h | 图像编辑和标注工具 |
| 取色 | ColorPicker.cpp/h | 屏幕取色功能 |
| 贴图 | PinWindow.cpp/h | 图片贴图到桌面 |
| 设置 | SettingsDialog.cpp/h | 设置界面 |
| 录屏 | ScreenRecorder.cpp/h | 屏幕录制和 GIF 导出 |
| 多语言 | Localization.cpp/h | 国际化支持 |
| 平台 | Platform.cpp/h | 跨平台抽象层 |
| 日志 | Logger.cpp/h | 日志记录系统 |

---

## 编码规范

### 命名规范

#### 文件命名
- 头文件：`ClassName.h`
- 源文件：`ClassName.cpp`
- 使用 PascalCase

#### 类命名
```cpp
class ScreenCapture { };      // PascalCase
class ColorPicker { };
```

#### 函数命名
```cpp
void StartCapture();          // PascalCase
bool IsRecording() const;
```

#### 变量命名
```cpp
// 成员变量：m_ 前缀 + camelCase
class MyClass {
    int m_width;
    std::wstring m_fileName;
    Bitmap* m_pBitmap;        // 指针：p 前缀
};

// 局部变量：camelCase
int screenWidth = 1920;
std::wstring filePath = L"...";

// 全局变量：g_ 前缀 + camelCase
HINSTANCE g_hInstance;
SnipXApp* g_pApp;

// 常量：k 前缀 + PascalCase 或全大写
const int kMaxAnnotations = 1000;
const wchar_t* kDefaultPath = L"C:\\Screenshots";
#define MAX_PATH_LENGTH 260
```

#### 枚举命名
```cpp
// 枚举类型：PascalCase
enum ToolType {
    TOOL_SELECT,              // 枚举值：全大写 + 下划线
    TOOL_RECTANGLE,
    TOOL_ELLIPSE
};

// 或使用 enum class（推荐）
enum class ImageFormat {
    PNG,
    JPEG,
    BMP
};
```

### 代码风格

#### 缩进和空格
- 使用 4 空格缩进（不使用 Tab）
- 运算符两侧加空格
- 逗号后加空格

```cpp
// 正确
int result = a + b;
Function(arg1, arg2, arg3);

// 错误
int result=a+b;
Function(arg1,arg2,arg3);
```

#### 大括号
- 大括号另起一行（Allman 风格）

```cpp
// 正确
void Function()
{
    if (condition)
    {
        // ...
    }
    else
    {
        // ...
    }
}

// 错误
void Function() {
    if (condition) {
        // ...
    } else {
        // ...
    }
}
```

#### 注释
- 使用中文或英文注释
- 关键逻辑必须注释
- 公共 API 使用 Doxygen 风格

```cpp
/**
 * @brief 启动屏幕截图
 * 
 * 创建全屏覆盖窗口，允许用户选择截图区域。
 * 
 * @return 成功返回 true，失败返回 false
 */
bool StartCapture();

// 单行注释：说明下面代码的作用
int width = GetSystemMetrics(SM_CXSCREEN);

/* 
 * 多行注释：
 * 用于复杂逻辑的详细说明
 */
```

#### 头文件保护
```cpp
#ifndef CLASSNAME_H
#define CLASSNAME_H

// 内容

#endif // CLASSNAME_H
```

#### 包含顺序
```cpp
// 1. 对应的头文件
#include "ClassName.h"

// 2. C 系统头文件
#include <windows.h>
#include <stdio.h>

// 3. C++ 标准库
#include <string>
#include <vector>

// 4. 第三方库
#include <gdiplus.h>

// 5. 项目内其他头文件
#include "Config.h"
#include "Logger.h"
```

### 最佳实践

#### 资源管理
```cpp
// 使用 RAII 管理资源
class BitmapGuard {
    Bitmap* m_bitmap;
public:
    BitmapGuard(Bitmap* bmp) : m_bitmap(bmp) {}
    ~BitmapGuard() { delete m_bitmap; }
    Bitmap* Get() { return m_bitmap; }
};

// 或使用智能指针
std::unique_ptr<Bitmap> bitmap(new Bitmap(width, height));
```

#### 错误处理
```cpp
// 检查返回值
if (!Initialize())
{
    LOG_ERROR(L"Failed to initialize");
    return false;
}

// 检查指针
if (!m_pBitmap)
{
    LOG_ERROR(L"Bitmap is null");
    return;
}

// 检查 Windows API 返回值
HWND hwnd = CreateWindow(...);
if (!hwnd)
{
    DWORD error = GetLastError();
    LOG_ERROR(L"CreateWindow failed: %d", error);
    return false;
}
```

#### 日志记录
```cpp
// 使用日志宏
LOG_DEBUG(L"Debug info: %d", value);
LOG_INFO(L"Operation started");
LOG_WARNING(L"Potential issue detected");
LOG_ERROR(L"Operation failed: %s", errorMsg);

// 关键操作记录日志
void StartCapture()
{
    LOG_INFO(L"Starting screen capture");
    
    if (!CreateCaptureWindow())
    {
        LOG_ERROR(L"Failed to create capture window");
        return;
    }
    
    LOG_INFO(L"Capture window created successfully");
}
```

#### 性能考虑
```cpp
// 避免不必要的拷贝
void ProcessBitmap(const Bitmap* bitmap);  // 传指针
void ProcessData(const std::vector<int>& data);  // 传引用

// 使用移动语义
std::vector<Annotation*> GetAnnotations()
{
    return std::move(m_annotations);
}

// 缓存计算结果
int GetWidth()
{
    if (m_widthCached)
        return m_cachedWidth;
    
    m_cachedWidth = CalculateWidth();
    m_widthCached = true;
    return m_cachedWidth;
}
```

---

## 开发流程

### 1. 创建分支

```bash
# 从 main 分支创建功能分支
git checkout main
git pull origin main
git checkout -b feature/your-feature-name

# 或创建修复分支
git checkout -b fix/bug-description
```

### 2. 开发功能

1. 编写代码
2. 添加日志
3. 处理错误
4. 编写注释

### 3. 测试

```cmd
# 编译
build.bat

# 运行
bin\SnipX.exe

# 测试功能
# 参考 TODO.md 中的测试任务清单
```

### 4. 提交代码

```bash
# 查看修改
git status
git diff

# 添加文件
git add src/NewFile.cpp src/NewFile.h

# 提交
git commit -m "feat: add new feature"

# 推送
git push origin feature/your-feature-name
```

### 5. 创建 Pull Request

1. 访问 GitHub 仓库
2. 点击 "New Pull Request"
3. 选择你的分支
4. 填写 PR 描述
5. 等待代码审查

### 提交信息规范

使用 Conventional Commits 格式：

```
<type>(<scope>): <subject>

<body>

<footer>
```

**类型（type）**：
- `feat`: 新功能
- `fix`: 修复 bug
- `docs`: 文档更新
- `style`: 代码格式（不影响功能）
- `refactor`: 重构
- `perf`: 性能优化
- `test`: 测试相关
- `chore`: 构建/工具相关

**示例**：
```
feat(editor): add blur tool

Implement blur annotation tool with adjustable strength.

Closes #123
```

---

## 调试技巧

### Visual Studio 调试

1. **设置断点**：
   - 在代码行左侧单击
   - 或按 F9

2. **启动调试**：
   - 按 F5 开始调试
   - 按 Ctrl+F5 不调试运行

3. **调试窗口**：
   - 局部变量：查看当前作用域变量
   - 监视：添加自定义表达式
   - 调用堆栈：查看函数调用链
   - 输出：查看调试输出

4. **调试技巧**：
   ```cpp
   // 输出调试信息
   OutputDebugStringW(L"Debug message\n");
   
   // 条件断点
   // 右键断点 -> 条件 -> 输入条件表达式
   
   // 数据断点
   // 右键变量 -> 中断条件 -> 值更改时中断
   ```

### 日志调试

```cpp
// 启用调试日志
Logger::GetInstance().SetLevel(LOG_DEBUG);

// 添加调试日志
LOG_DEBUG(L"Variable value: %d", value);
LOG_DEBUG(L"Entering function: %s", __FUNCTIONW__);
LOG_DEBUG(L"Pointer: %p", ptr);
```

### 内存泄漏检测

```cpp
// 在 main.cpp 开头添加
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

### GDI+ 对象检测

```cpp
// 跟踪 GDI+ 对象创建和销毁
class GdiObjectTracker
{
    static int s_count;
public:
    GdiObjectTracker() { s_count++; }
    ~GdiObjectTracker() { s_count--; }
    static int GetCount() { return s_count; }
};

// 在析构函数中检查
~Editor()
{
    if (GdiObjectTracker::GetCount() > 0)
    {
        LOG_WARNING(L"GDI+ objects not released: %d", 
                    GdiObjectTracker::GetCount());
    }
}
```

### 性能分析

```cpp
// 简单计时器
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
void SlowFunction()
{
    Timer timer;
    // ... 执行操作
    LOG_INFO(L"Function took %.3f seconds", timer.Elapsed());
}
```

---

## 贡献指南

### 如何贡献

1. **报告 Bug**：
   - 在 GitHub Issues 创建 bug 报告
   - 提供详细的复现步骤
   - 附上截图或日志

2. **建议功能**：
   - 在 GitHub Issues 创建功能请求
   - 说明使用场景和预期效果

3. **提交代码**：
   - Fork 项目
   - 创建功能分支
   - 提交 Pull Request

### 代码审查

提交 PR 后，维护者会进行代码审查，检查：

- [ ] 代码风格符合规范
- [ ] 功能正确实现
- [ ] 没有引入新 bug
- [ ] 性能影响可接受
- [ ] 文档已更新
- [ ] 测试已通过

### 社区规范

- 尊重他人
- 建设性反馈
- 保持专业
- 遵守开源协议

---

## 常见问题

### Q: 如何添加新的标注工具？

A: 参考 `API_DOCUMENTATION.md` 中的"扩展示例：添加自定义标注工具"

### Q: 如何修改默认热键？

A: 修改 `Config.cpp` 中的 `DEFAULT_HOTKEY` 常量

### Q: 如何支持新的图像格式？

A: 扩展 `Editor::Save()` 方法，添加新格式的编码器

### Q: 如何添加新的语言？

A: 在 `Localization.cpp` 中添加新语言的字符串表

### Q: 编译时出现链接错误？

A: 检查是否正确链接了所有必需的库（gdiplus.lib, shell32.lib 等）

### Q: 如何减小可执行文件大小？

A: 参考 `PERFORMANCE_OPTIMIZATION.md` 中的"体积优化"章节

---

## 资源链接

- **项目主页**: https://github.com/yourusername/SnipX
- **问题跟踪**: https://github.com/yourusername/SnipX/issues
- **Wiki**: https://github.com/yourusername/SnipX/wiki
- **讨论区**: https://github.com/yourusername/SnipX/discussions

### 技术文档

- [Windows API 文档](https://docs.microsoft.com/en-us/windows/win32/api/)
- [GDI+ 文档](https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-gdi-start)
- [CMake 文档](https://cmake.org/documentation/)

### 学习资源

- [C++ 参考](https://en.cppreference.com/)
- [Win32 编程教程](https://www.winprog.org/tutorial/)
- [GDI+ 教程](https://www.codeproject.com/Articles/3001/GDI-Tutorial)

---

## 联系方式

- **邮件**: your.email@example.com
- **GitHub**: @yourusername
- **Discord**: SnipX Community

---

**感谢你对 SnipX 的贡献！**

**最后更新**: 2024-01-15
