# SnipX 快速开始指南

## 5 分钟快速编译

### 步骤 1：准备图标（可选）

**方法 A：使用 Python 生成图标（推荐）**
```bash
pip install Pillow
python generate_icon.py
```

**方法 B：临时跳过图标**

编辑 `res/SnipX.rc`，注释掉图标行：
```rc
// IDI_MAINICON            ICON                    "icon.ico"
```

同时修改 `src/TrayIcon.cpp` 第 30 行：
```cpp
// 原代码：
m_nid.hIcon = LoadIcon(pApp->GetInstance(), MAKEINTRESOURCE(IDI_MAINICON));

// 改为：
m_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
```

### 步骤 2：编译项目

**使用 Visual Studio：**
```cmd
# 打开 "Developer Command Prompt for VS"
cd C:\Users\yyyzz\Desktop\SnipX
build.bat
```

**使用 MinGW：**
```cmd
cd C:\Users\yyyzz\Desktop\SnipX
build.bat
```

### 步骤 3：运行测试

```cmd
bin\SnipX.exe
```

**测试功能：**
1. ✅ 程序启动后托盘显示图标
2. ✅ 按 Alt+X 触发截图
3. ✅ 双击托盘图标触发截图
4. ✅ 拖拽选择截图区域
5. ✅ 看到放大镜和尺寸信息
6. ✅ 松开鼠标确认截图

## 常见问题

### Q1: 编译失败 - 找不到 gdiplus.lib

**解决：** 安装 Windows SDK 或完整的 Visual Studio

### Q2: 编译失败 - 找不到 icon.ico

**解决：** 使用步骤 1 的方法 B 跳过图标

### Q3: 运行时没有托盘图标

**解决：** 检查是否使用了系统图标（方法 B）

### Q4: 按 Alt+X 没反应

**解决：** 
- 检查是否有其他程序占用了该快捷键
- 尝试以管理员身份运行

### Q5: 截图后没有编辑器

**解决：** 编辑器功能还在开发中，当前版本会显示提示框

## 开发模式

### 调试编译

**MSVC：**
```cmd
cl /c /Od /Zi /MDd /W3 /nologo /I"src" /Fo"obj\\" src\*.cpp
link /OUT:bin\SnipX.exe /DEBUG /SUBSYSTEM:WINDOWS obj\*.obj gdiplus.lib shell32.lib user32.lib gdi32.lib ole32.lib dwmapi.lib
```

**MinGW：**
```cmd
g++ -g -O0 -mwindows -I"src" src\*.cpp -o bin\SnipX.exe -lgdiplus -lshell32 -luser32 -lgdi32 -lole32 -ldwmapi
```

### 使用 Visual Studio 调试

1. 打开 Visual Studio
2. 文件 → 打开 → 项目/解决方案
3. 选择 `CMakeLists.txt`
4. 设置断点
5. F5 开始调试

## 下一步

### 查看文档
- `README.md` - 项目概述
- `ARCHITECTURE.md` - 架构设计
- `TODO.md` - 开发任务
- `COMPILE.md` - 详细编译指南

### 参与开发
1. 查看 `TODO.md` 选择任务
2. 阅读 `ARCHITECTURE.md` 了解架构
3. 开始编码！

### 反馈问题
- 在 GitHub Issues 提交问题
- 提供详细的错误信息和系统环境

## 体积检查

编译完成后，检查文件大小：

```cmd
dir bin\SnipX.exe
```

**目标：** < 1,048,576 字节（1 MB）

**当前预期：** ~500 KB - 800 KB（未压缩）

**使用 UPX 压缩后：** ~200 KB - 400 KB

## 性能测试

### 启动速度
```cmd
# 使用 PowerShell 测量
Measure-Command { bin\SnipX.exe }
```

**目标：** < 500ms

### 内存占用
1. 运行 SnipX
2. 打开任务管理器
3. 查看内存占用

**目标：** < 10 MB（空闲）

## 贡献代码

### 代码风格
```cpp
// 类名：PascalCase
class ScreenCapture { ... };

// 函数名：PascalCase
void StartCapture() { ... }

// 变量名：camelCase
int screenWidth = 0;

// 成员变量：m_ 前缀
int m_width;
```

### 提交规范
```
feat: 添加矩形标注工具
fix: 修复多显示器截图偏移问题
docs: 更新编译文档
perf: 优化绘制性能
```

## 获取帮助

- 📖 阅读文档：`docs/` 目录
- 💬 提问讨论：GitHub Discussions
- 🐛 报告 Bug：GitHub Issues
- 📧 联系作者：（待添加）

---

**开始你的 SnipX 之旅吧！** 🚀
