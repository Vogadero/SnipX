# SnipX 性能优化指南

本文档记录 SnipX 的性能优化策略和实施方案，目标是实现 <1MB 体积和流畅的用户体验。

## 目录

- [体积优化](#体积优化)
- [内存优化](#内存优化)
- [CPU 优化](#cpu优化)
- [启动优化](#启动优化)
- [渲染优化](#渲染优化)

---

## 体积优化

### 当前状态

- 目标：< 1MB
- 策略：纯 Win32 API + GDI+，避免大型依赖

### 优化措施

#### 1. 编译器优化选项

**MSVC 编译器**：
```bat
cl /O2 /GL /EHsc /MD /W3 /nologo
link /LTCG /OPT:REF /OPT:ICF
```

- `/O2`: 最大速度优化
- `/GL`: 全程序优化
- `/LTCG`: 链接时代码生成
- `/OPT:REF`: 移除未引用函数
- `/OPT:ICF`: 合并相同函数

**MinGW 编译器**：
```bat
g++ -O3 -s -static -mwindows
```

- `-O3`: 最高优化级别
- `-s`: 移除符号表
- `-static`: 静态链接（减少依赖）

#### 2. UPX 压缩

使用 UPX 压缩最终可执行文件：

```bat
upx --best --lzma bin\SnipX.exe
```

预期压缩率：50-70%

**注意**：某些杀毒软件可能误报，建议提供未压缩版本。

#### 3. 减少 STL 使用

**问题**：STL 模板会增加代码体积

**优化**：
- 使用 Win32 API 替代 STL 容器
- 用 `std::vector` 替代 `std::list`（更紧凑）
- 避免过度使用模板

**示例**：

```cpp
// 优化前：使用 std::map
std::map<int, std::wstring> m_items;

// 优化后：使用简单数组
struct Item {
    int id;
    wchar_t name[64];
};
std::vector<Item> m_items;
```

#### 4. 字符串优化

**问题**：`std::wstring` 动态分配增加体积

**优化**：
- 固定长度字符串使用 `wchar_t[]`
- 字符串常量使用 `const wchar_t*`
- 避免不必要的字符串拷贝

```cpp
// 优化前
std::wstring GetTitle() {
    return std::wstring(L"SnipX - ") + m_version;
}

// 优化后
const wchar_t* GetTitle() {
    static wchar_t title[64];
    swprintf_s(title, L"SnipX - %s", m_version);
    return title;
}
```

#### 5. 减少虚函数

**问题**：虚函数表增加体积

**优化**：
- 仅在必要时使用虚函数（如标注基类）
- 考虑使用函数指针或枚举替代

```cpp
// 优化前：每个工具一个类
class RectangleTool : public Tool { virtual void Draw(); };
class EllipseTool : public Tool { virtual void Draw(); };

// 优化后：统一工具类
enum ToolType { RECT, ELLIPSE };
class Tool {
    ToolType m_type;
    void Draw() {
        switch(m_type) {
            case RECT: DrawRectangle(); break;
            case ELLIPSE: DrawEllipse(); break;
        }
    }
};
```

#### 6. 资源优化

- 图标使用单一 `.ico` 文件（多尺寸）
- 避免嵌入大型资源
- 工具栏图标使用 GDI+ 绘制而非位图

---

## 内存优化

### 目标

- 空闲时内存占用 < 10MB
- 截图时内存占用 < 50MB（1920x1080）

### 优化措施

#### 1. 位图管理

**问题**：截图位图占用大量内存

**优化**：
- 及时释放不再使用的位图
- 使用引用计数管理位图生命周期
- 编辑器关闭时立即释放位图

```cpp
// 位图智能指针
class BitmapPtr {
    Bitmap* m_ptr;
    int* m_refCount;
public:
    BitmapPtr(Bitmap* p) : m_ptr(p), m_refCount(new int(1)) {}
    ~BitmapPtr() {
        if (--(*m_refCount) == 0) {
            delete m_ptr;
            delete m_refCount;
        }
    }
    // 拷贝构造和赋值
};
```

#### 2. 标注对象池

**问题**：频繁创建/删除标注对象导致内存碎片

**优化**：
- 使用对象池复用标注对象
- 预分配常用对象

```cpp
class AnnotationPool {
    std::vector<Annotation*> m_pool;
    std::vector<Annotation*> m_free;
    
public:
    Annotation* Allocate(ToolType type) {
        if (!m_free.empty()) {
            Annotation* ann = m_free.back();
            m_free.pop_back();
            ann->Reset(type);
            return ann;
        }
        return new Annotation(type);
    }
    
    void Free(Annotation* ann) {
        m_free.push_back(ann);
    }
};
```

#### 3. 延迟加载

**优化**：
- 取色器、录屏器等模块延迟创建
- 仅在使用时初始化

```cpp
ColorPicker* SnipXApp::GetColorPicker() {
    if (!m_pColorPicker) {
        m_pColorPicker = new ColorPicker(this);
    }
    return m_pColorPicker;
}
```

#### 4. 内存泄漏检测

**工具**：
- Visual Studio CRT 调试
- Valgrind（跨平台）
- Dr. Memory

**检测代码**：

```cpp
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // ... 程序代码
    _CrtDumpMemoryLeaks();
}
#endif
```

#### 5. 避免内存泄漏

**常见问题**：
- GDI+ 对象未释放
- COM 对象未 Release
- 窗口句柄未销毁

**检查清单**：
```cpp
// ✓ 释放 GDI+ 对象
delete bitmap;
delete graphics;
delete pen;
delete brush;

// ✓ 释放 GDI 对象
DeleteObject(hBitmap);
DeleteDC(hdc);

// ✓ 销毁窗口
DestroyWindow(hwnd);

// ✓ COM 对象
pInterface->Release();
```

---

## CPU 优化

### 目标

- 空闲时 CPU 占用 < 1%
- 截图时 CPU 占用 < 20%

### 优化措施

#### 1. 双缓冲绘制

**问题**：直接绘制到窗口导致闪烁和高 CPU

**优化**：使用内存 DC 双缓冲

```cpp
void Editor::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);
    
    // 创建内存 DC
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    // 绘制到内存 DC
    Graphics g(memDC);
    DrawContent(&g);
    
    // 一次性复制到窗口
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    
    // 清理
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    
    EndPaint(m_hwnd, &ps);
}
```

#### 2. 减少重绘

**优化**：
- 仅重绘变化区域
- 使用 `InvalidateRect()` 而非 `InvalidateWindow()`
- 合并连续的重绘请求

```cpp
// 仅重绘工具栏区域
RECT toolbarRect = {0, 0, width, 50};
InvalidateRect(m_hwnd, &toolbarRect, FALSE);
```

#### 3. 鼠标移动优化

**问题**：鼠标移动事件频繁触发重绘

**优化**：
- 限制重绘频率（节流）
- 仅在必要时重绘

```cpp
void Editor::OnMouseMove(int x, int y) {
    // 节流：每 16ms 最多重绘一次（60 FPS）
    DWORD now = GetTickCount();
    if (now - m_lastRedraw < 16) {
        return;
    }
    m_lastRedraw = now;
    
    // 仅在绘制模式下重绘
    if (m_isDrawing) {
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}
```

#### 4. 位图缩放优化

**问题**：实时缩放位图消耗 CPU

**优化**：
- 缓存缩放后的位图
- 使用低质量插值（快速模式）

```cpp
// 缓存缩放位图
Bitmap* m_scaledBitmap = nullptr;
float m_lastScale = 1.0f;

void DrawScaledBitmap(Graphics* g, float scale) {
    if (scale != m_lastScale || !m_scaledBitmap) {
        delete m_scaledBitmap;
        int w = (int)(m_originalBitmap->GetWidth() * scale);
        int h = (int)(m_originalBitmap->GetHeight() * scale);
        m_scaledBitmap = new Bitmap(w, h);
        
        Graphics gTemp(m_scaledBitmap);
        gTemp.SetInterpolationMode(InterpolationModeNearestNeighbor); // 快速
        gTemp.DrawImage(m_originalBitmap, 0, 0, w, h);
        
        m_lastScale = scale;
    }
    
    g->DrawImage(m_scaledBitmap, 0, 0);
}
```

#### 5. 马赛克/模糊优化

**问题**：像素级处理慢

**优化**：
- 降低处理分辨率
- 使用查找表
- 多线程处理

```cpp
// 快速马赛克：降采样 + 放大
void ApplyMosaic(Bitmap* bitmap, RECT rect, int blockSize) {
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    
    // 缩小
    int smallW = w / blockSize;
    int smallH = h / blockSize;
    Bitmap* small = new Bitmap(smallW, smallH);
    Graphics g1(small);
    g1.DrawImage(bitmap, 0, 0, smallW, smallH, 
                 rect.left, rect.top, w, h, UnitPixel);
    
    // 放大回去（像素化效果）
    Graphics g2(bitmap);
    g2.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g2.DrawImage(small, rect.left, rect.top, w, h);
    
    delete small;
}
```

---

## 启动优化

### 目标

- 冷启动时间 < 500ms
- 热启动时间 < 100ms

### 优化措施

#### 1. 延迟初始化

**优化**：
- 仅初始化核心模块
- 其他模块按需加载

```cpp
bool SnipXApp::Initialize() {
    // 核心模块：立即初始化
    InitializeGDIPlus();
    m_pConfig = new Config();
    m_pHotkeyMgr = new HotkeyManager(this);
    m_pTrayIcon = new TrayIcon(this);
    
    // 非核心模块：延迟初始化
    m_pCapture = nullptr;
    m_pEditor = nullptr;
    m_pColorPicker = nullptr;
    m_pRecorder = nullptr;
    
    return true;
}

void SnipXApp::StartCapture() {
    if (!m_pCapture) {
        m_pCapture = new ScreenCapture(this);
    }
    m_pCapture->Start();
}
```

#### 2. 配置文件优化

**优化**：
- 使用二进制格式而非 INI（更快）
- 缓存配置，减少磁盘读取

```cpp
// 二进制配置格式
struct ConfigData {
    DWORD hotkey;
    int startupMode;
    bool autoStart;
    // ... 其他字段
};

bool Config::LoadBinary() {
    HANDLE hFile = CreateFile(path, GENERIC_READ, ...);
    ConfigData data;
    DWORD bytesRead;
    ReadFile(hFile, &data, sizeof(data), &bytesRead, NULL);
    CloseHandle(hFile);
    
    // 应用配置
    m_hotkey = data.hotkey;
    // ...
}
```

#### 3. 减少 DLL 依赖

**优化**：
- 静态链接 CRT（`/MT`）
- 避免加载不必要的 DLL

```bat
# 静态链接
cl /MT /O2 ...
```

---

## 渲染优化

### 优化措施

#### 1. GDI+ 性能设置

```cpp
void Editor::InitializeGraphics(Graphics* g) {
    // 性能优先
    g->SetSmoothingMode(SmoothingModeHighSpeed);
    g->SetInterpolationMode(InterpolationModeNearestNeighbor);
    g->SetPixelOffsetMode(PixelOffsetModeHighSpeed);
    g->SetCompositingQuality(CompositingQualityHighSpeed);
    
    // 质量优先（编辑完成后）
    // g->SetSmoothingMode(SmoothingModeAntiAlias);
    // g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
}
```

#### 2. 脏矩形优化

**优化**：仅重绘变化区域

```cpp
class Editor {
    std::vector<RECT> m_dirtyRects;
    
    void MarkDirty(RECT rect) {
        m_dirtyRects.push_back(rect);
    }
    
    void OnPaint() {
        for (const RECT& rect : m_dirtyRects) {
            // 仅绘制脏区域
            DrawRegion(rect);
        }
        m_dirtyRects.clear();
    }
};
```

#### 3. 图层缓存

**优化**：缓存不变的图层

```cpp
class Editor {
    Bitmap* m_backgroundLayer;  // 原始截图（不变）
    Bitmap* m_annotationLayer;  // 标注层（变化）
    
    void Render(Graphics* g) {
        // 绘制背景（缓存）
        g->DrawImage(m_backgroundLayer, 0, 0);
        
        // 绘制标注（实时）
        g->DrawImage(m_annotationLayer, 0, 0);
    }
    
    void OnAnnotationChanged() {
        // 仅重绘标注层
        Graphics g(m_annotationLayer);
        g.Clear(Color(0, 0, 0, 0));  // 透明
        for (auto ann : m_annotations) {
            ann->Draw(&g);
        }
    }
};
```

---

## 性能测试

### 测试工具

```cpp
class PerformanceMonitor {
    LARGE_INTEGER m_freq;
    
public:
    PerformanceMonitor() {
        QueryPerformanceFrequency(&m_freq);
    }
    
    double MeasureFunction(std::function<void()> func) {
        LARGE_INTEGER start, end;
        QueryPerformanceCounter(&start);
        
        func();
        
        QueryPerformanceCounter(&end);
        return (double)(end.QuadPart - start.QuadPart) / m_freq.QuadPart;
    }
    
    void LogMemoryUsage() {
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        
        LOG_INFO(L"Memory: Working Set = %.2f MB, Private = %.2f MB",
                 pmc.WorkingSetSize / 1024.0 / 1024.0,
                 pmc.PagefileUsage / 1024.0 / 1024.0);
    }
};
```

### 性能基准

| 操作 | 目标时间 | 当前时间 | 状态 |
|------|---------|---------|------|
| 启动 | < 500ms | - | 待测 |
| 截图 | < 100ms | - | 待测 |
| 保存 PNG | < 200ms | - | 待测 |
| 添加标注 | < 16ms | - | 待测 |
| 撤销/重做 | < 10ms | - | 待测 |

---

## 优化检查清单

### 编译时
- [ ] 启用编译器优化（`/O2` 或 `-O3`）
- [ ] 启用链接时优化（`/LTCG` 或 `-flto`）
- [ ] 移除调试符号（`/s`）
- [ ] 静态链接 CRT（`/MT`）
- [ ] 使用 UPX 压缩

### 代码层面
- [ ] 减少 STL 使用
- [ ] 优化字符串处理
- [ ] 减少虚函数
- [ ] 使用对象池
- [ ] 延迟初始化

### 渲染层面
- [ ] 双缓冲绘制
- [ ] 脏矩形优化
- [ ] 图层缓存
- [ ] 降低绘制质量（实时模式）
- [ ] 限制重绘频率

### 内存层面
- [ ] 及时释放位图
- [ ] 检测内存泄漏
- [ ] 使用智能指针
- [ ] 避免内存碎片

### 测试
- [ ] 性能基准测试
- [ ] 内存占用测试
- [ ] CPU 占用测试
- [ ] 启动时间测试
- [ ] 长时间运行测试

---

## 未来优化方向

1. **多线程**：
   - 后台保存
   - 异步截图
   - 并行图像处理

2. **GPU 加速**：
   - Direct2D 替代 GDI+
   - 硬件加速渲染

3. **算法优化**：
   - 更快的马赛克算法
   - 更快的模糊算法
   - 更快的碰撞检测

4. **缓存策略**：
   - 缩略图缓存
   - 历史记录缓存
   - 配置缓存

---

**最后更新**: 2024-01-15
