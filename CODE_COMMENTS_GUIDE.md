# SnipX 代码注释规范

本文档定义 SnipX 项目的代码注释标准和最佳实践。

## 注释原则

1. **清晰性**: 注释应该清晰、简洁、易懂
2. **必要性**: 只在需要的地方添加注释，代码应该自解释
3. **准确性**: 注释必须与代码保持同步
4. **完整性**: 公共 API 必须有完整的文档注释

## 注释类型

### 1. 文件头注释

每个源文件开头应包含文件说明：

```cpp
// SnipX - 极致轻量截图工具
// 文件名: ClassName.cpp
// 功能: 类的简要说明
// 作者: Your Name
// 日期: 2024-01-15

#include "ClassName.h"
```

### 2. 类注释

使用 Doxygen 风格注释类：

```cpp
/**
 * @brief 截图引擎类
 * 
 * 负责屏幕捕获、窗口检测和选区管理。
 * 支持全屏截图、区域截图和窗口截图。
 * 
 * @note 线程安全性：此类不是线程安全的，应在主线程使用
 * 
 * @example
 * ScreenCapture capture(app);
 * capture.Start();
 */
class ScreenCapture
{
    // ...
};
```

### 3. 函数注释

公共函数使用 Doxygen 风格：

```cpp
/**
 * @brief 启动屏幕截图
 * 
 * 创建全屏覆盖窗口，允许用户拖拽选择截图区域。
 * 支持窗口自动检测和放大镜辅助。
 * 
 * @return 成功返回 true，失败返回 false
 * 
 * @note 此函数会阻塞直到用户完成选择或取消
 * 
 * @see CaptureFullScreen(), CaptureLastSelection()
 */
bool Start();
```

私有函数使用简单注释：

```cpp
// 创建截图窗口
// 返回：窗口句柄，失败返回 NULL
HWND CreateCaptureWindow();
```

### 4. 参数注释

复杂参数需要详细说明：

```cpp
/**
 * @brief 保存截图到文件
 * 
 * @param path 保存路径，必须是绝对路径
 * @param format 图像格式（PNG/JPEG/BMP）
 * @param quality JPEG 质量（1-100），仅对 JPEG 有效
 * 
 * @return 成功返回 true，失败返回 false
 * 
 * @throws 不抛出异常，错误通过返回值和日志报告
 */
bool Save(const std::wstring& path, ImageFormat format, int quality = 90);
```

### 5. 成员变量注释

```cpp
class Editor
{
private:
    HWND m_hwnd;                    // 编辑器窗口句柄
    Bitmap* m_pBitmap;              // 当前编辑的位图
    std::vector<Annotation*> m_annotations;  // 标注对象列表
    ToolType m_currentTool;         // 当前选中的工具
    COLORREF m_color;               // 当前颜色（RGB）
    int m_thickness;                // 当前粗细（像素）
    bool m_isDrawing;               // 是否正在绘制
    
    // 撤销/重做栈
    std::vector<Annotation*> m_undoStack;
    std::vector<Annotation*> m_redoStack;
};
```

### 6. 常量和枚举注释

```cpp
// 工具类型枚举
enum ToolType
{
    TOOL_SELECT,      // 选择工具 - 用于选中和编辑标注
    TOOL_RECTANGLE,   // 矩形工具 - 绘制矩形边框
    TOOL_ELLIPSE,     // 椭圆工具 - 绘制椭圆边框
    TOOL_ARROW,       // 箭头工具 - 绘制箭头指示
    TOOL_LINE,        // 直线工具 - 绘制直线
    TOOL_PENCIL,      // 铅笔工具 - 自由绘制
    TOOL_TEXT,        // 文字工具 - 添加文本
    TOOL_MOSAIC,      // 马赛克工具 - 像素化模糊
    TOOL_BLUR         // 模糊工具 - 高斯模糊
};

// 默认配置常量
const int DEFAULT_THICKNESS = 2;        // 默认线条粗细
const COLORREF DEFAULT_COLOR = RGB(255, 0, 0);  // 默认颜色（红色）
const int DEFAULT_JPEG_QUALITY = 90;    // 默认 JPEG 质量
```

### 7. 算法注释

复杂算法需要详细说明：

```cpp
/**
 * 应用马赛克效果
 * 
 * 算法：
 * 1. 将区域划分为 blockSize x blockSize 的块
 * 2. 计算每个块的平均颜色
 * 3. 用平均颜色填充整个块
 * 
 * 时间复杂度：O(width * height)
 * 空间复杂度：O(1)
 */
void ApplyMosaic(Bitmap* bitmap, RECT rect, int blockSize)
{
    // 遍历每个块
    for (int y = rect.top; y < rect.bottom; y += blockSize)
    {
        for (int x = rect.left; x < rect.right; x += blockSize)
        {
            // 计算块的平均颜色
            Color avgColor = CalculateAverageColor(bitmap, x, y, blockSize);
            
            // 填充块
            FillBlock(bitmap, x, y, blockSize, avgColor);
        }
    }
}
```

### 8. TODO 注释

```cpp
// TODO: 添加对 WebP 格式的支持
// TODO(username): 优化大图片的保存速度
// FIXME: 高 DPI 下工具栏图标模糊
// HACK: 临时解决方案，需要重构
// NOTE: 此处代码依赖 Windows 10+ API
// WARNING: 此函数不是线程安全的
```

### 9. 代码块注释

```cpp
void ComplexFunction()
{
    // 第一步：初始化资源
    InitializeResources();
    
    // 第二步：处理数据
    {
        // 创建临时缓冲区
        Buffer buffer(1024);
        
        // 读取数据
        ReadData(&buffer);
        
        // 处理数据
        ProcessData(&buffer);
    }
    
    // 第三步：清理资源
    CleanupResources();
}
```

### 10. 条件编译注释

```cpp
#ifdef _DEBUG
    // Debug 模式：启用详细日志
    Logger::GetInstance().SetLevel(LOG_DEBUG);
#else
    // Release 模式：仅记录错误
    Logger::GetInstance().SetLevel(LOG_ERROR);
#endif

#ifdef SNIPX_PLATFORM_WINDOWS
    // Windows 特定代码
    HWND hwnd = CreateWindowW(...);
#elif defined(SNIPX_PLATFORM_MACOS)
    // macOS 特定代码
    NSWindow* window = [[NSWindow alloc] init];
#endif
```

## 注释最佳实践

### ✅ 好的注释

```cpp
// 计算两点之间的距离
double Distance(Point p1, Point p2)
{
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

// 检查点是否在矩形内
// 边界上的点被认为在矩形内
bool IsPointInRect(Point p, RECT rect)
{
    return p.x >= rect.left && p.x <= rect.right &&
           p.y >= rect.top && p.y <= rect.bottom;
}
```

### ❌ 不好的注释

```cpp
// 加 1
count++;  // 不必要的注释

// 这个函数很复杂
void DoSomething()  // 没有说明复杂在哪里
{
    // ...
}

// 循环
for (int i = 0; i < 10; i++)  // 显而易见的注释
{
    // ...
}
```

## 文档生成

使用 Doxygen 生成 API 文档：

### 1. 安装 Doxygen

下载：https://www.doxygen.nl/download.html

### 2. 创建配置文件

```bash
doxygen -g Doxyfile
```

### 3. 编辑 Doxyfile

```
PROJECT_NAME           = "SnipX"
PROJECT_BRIEF          = "极致轻量的 Windows 截图工具"
OUTPUT_DIRECTORY       = docs
INPUT                  = src
RECURSIVE              = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
```

### 4. 生成文档

```bash
doxygen Doxyfile
```

## 注释检查清单

在提交代码前，检查：

- [ ] 所有公共类都有类注释
- [ ] 所有公共函数都有函数注释
- [ ] 复杂算法有详细说明
- [ ] 重要的成员变量有注释
- [ ] 没有过时的注释
- [ ] 没有注释掉的代码（应删除）
- [ ] TODO/FIXME 有明确的说明

## 注释模板

### 类模板

```cpp
/**
 * @brief [类的简要说明]
 * 
 * [类的详细说明，包括功能、用途、使用场景]
 * 
 * @note [注意事项]
 * @warning [警告信息]
 * 
 * @example
 * [使用示例代码]
 */
class ClassName
{
};
```

### 函数模板

```cpp
/**
 * @brief [函数简要说明]
 * 
 * [函数详细说明]
 * 
 * @param param1 [参数1说明]
 * @param param2 [参数2说明]
 * 
 * @return [返回值说明]
 * 
 * @note [注意事项]
 * @see [相关函数]
 */
ReturnType FunctionName(Type1 param1, Type2 param2);
```

## 多语言注释

对于国际化项目，可以使用中英文双语注释：

```cpp
/**
 * @brief 启动屏幕截图 / Start screen capture
 * 
 * 创建全屏覆盖窗口，允许用户拖拽选择截图区域。
 * Creates a fullscreen overlay window for user to select capture area.
 * 
 * @return 成功返回 true / Returns true on success
 */
bool Start();
```

## 注释维护

1. **代码修改时同步更新注释**
2. **定期审查注释的准确性**
3. **删除过时的注释**
4. **删除注释掉的代码**

---

**记住：好的代码应该自解释，注释是补充而非替代！**

**最后更新**: 2024-01-15
