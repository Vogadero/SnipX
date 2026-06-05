# SnipX - 极致轻量截图工具

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%207%2B-lightgrey.svg)](#系统要求)
[![Language](https://img.shields.io/badge/language-C%2B%2B17-orange.svg)](https://isocpp.org/)
[![Build](https://img.shields.io/badge/build-MSVC%20%7C%20MinGW%20%7C%20CMake-brightgreen.svg)](#编译构建)
[![Status](https://img.shields.io/badge/status-Stable-brightgreen.svg)](#项目状态)

> SnipX 是一款基于纯 Win32 C++17、GDI+ 和系统 API 开发的轻量级截图工具，聚焦快速截图、标注、贴图、取色、录屏与高可移植发布。

[English](README_EN.md) | 简体中文

## 项目状态

当前版本为 v1.0.0 首个正式版本。核心截图、标注、设置、贴图、取色、录屏、文档、构建脚本和 GitHub Release 工作流均已完成；高 DPI、多显示器不同 DPI 和旧版 Windows 兼容性仍建议在真实设备上持续验证。

## 功能特性

### 截图能力

- 区域截图：拖拽选择任意屏幕区域。
- 全屏截图：快速截取虚拟桌面完整画面。
- 窗口检测：自动识别窗口边界。
- 滚动截图：支持简化长截图流程。
- 多显示器：支持虚拟桌面坐标和多屏截图场景。
- 上次选区：可复用最近一次截图区域。

### 标注与编辑

- 矩形、椭圆、箭头、直线、铅笔、文字。
- 马赛克、模糊、高亮、序号标签。
- 标注对象选择、移动、删除、撤销/重做。
- 工具栏、颜色选择器、粗细选择器和提示信息。
- 保存为 PNG/JPG/BMP，支持 JPG 质量配置。
- 复制到剪贴板，支持双击复制并退出。

### 高级功能

- 取色器：支持 HEX/RGB/HSL/HSV 显示与复制，`R/H` 键循环切换显示模式。
- 贴图：截图贴回桌面，支持拖拽、缩放、透明度调节和鼠标穿透。
- 录屏：支持屏幕帧序列录制和 GIF 导出。
- OCR：已提供文字识别入口，当前为可扩展占位实现。
- 图片上传：已提供图床上传入口，当前导出待上传临时图片。
- 多语言：已接入轻量本地化模块，支持中文/英文文本切换。
- 跨平台适配层：已提供 Windows/macOS/Linux 平台识别与能力判断，Win32 UI 当前以 Windows 为完整功能平台。

### 设置与系统集成

- 系统托盘图标与托盘菜单。
- 启动模式配置：驻留托盘 / 启动即截图。
- 托盘图标显示/隐藏。
- 默认保存格式、保存目录、文件名前缀、JPG 质量。
- 自动保存即时生效，自动保存目录不存在时会尝试创建。
- 默认标注颜色和线宽配置。
- Windows 7/8/8.1/10/11 manifest 兼容性声明。

### 工程化与发布

- `build.bat`：自动选择 MSVC 或 MinGW 构建。
- `CMakeLists.txt`：支持 CMake 构建。
- `release.bat`：发布包构建脚本。
- `installer.iss`：Inno Setup 安装包配置。
- `.github/workflows/release.yml`：GitHub Release 工作流。
- `optimize.bat`：体积优化与 UPX 压缩脚本。
- `docs/`：VitePress 官网/文档站。

## 快速开始

### 直接使用

1. 下载或构建 `SnipX.exe`。
2. 双击启动程序。
3. 通过托盘菜单或快捷键开始截图。
4. 在编辑器中标注、保存、复制或贴图。

### 常用快捷键

| 快捷键 | 功能 |
| --- | --- |
| `Alt + X` | 区域截图 |
| `Alt + F` | 全屏截图 |
| `Esc` | 取消截图或关闭编辑器 |
| `Enter` | 确认截图选区 |
| `Ctrl + S` | 保存截图 |
| `Ctrl + C` | 复制截图 |
| `Ctrl + Z / Ctrl + Y` | 撤销 / 重做 |
| `Delete` | 删除选中标注 |
| `R / H` | 取色器显示模式切换 |
| `1 / 2 / 3 / 4` | 取色器复制 HEX / RGB / HSL / HSV |

更多说明见 [用户手册](USER_MANUAL.md)。

## 系统要求

- 操作系统：Windows 7 / 8 / 8.1 / 10 / 11。
- 架构：x86 或 x64，推荐 x64。
- 运行依赖：使用系统自带 Win32 API、GDI+ 和 Common Controls，无需额外运行时。
- 编译环境：Visual Studio 2019+、MinGW-w64 或 CMake 3.15+。

## 编译构建

### 使用 `build.bat`

```cmd
build.bat
```

脚本会优先检测 MSVC，未检测到时尝试使用 MinGW-w64。

### 使用 CMake

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

输出文件位于 `build/bin/` 或 `bin/`。

### 生成图标

如需重新生成应用图标：

```cmd
python generate_icon.py
```

该脚本会生成：

- `res/icon.ico`
- `res/icon_preview.png`

## 项目结构

```text
SnipX/
├── src/                         # 源代码
│   ├── main.cpp                 # 程序入口
│   ├── SnipX.cpp/h              # 主应用类
│   ├── Config.cpp/h             # 配置管理
│   ├── HotkeyManager.cpp/h      # 热键管理
│   ├── TrayIcon.cpp/h           # 系统托盘
│   ├── ScreenCapture.cpp/h      # 截图引擎
│   ├── ScreenRecorder.cpp/h     # 录屏/GIF 导出
│   ├── Editor.cpp/h             # 截图编辑器
│   ├── ColorPicker.cpp/h        # 取色器
│   ├── PinWindow.cpp/h          # 贴图窗口
│   ├── SettingsDialog.cpp/h     # 设置界面
│   ├── Localization.cpp/h       # 本地化
│   ├── Platform.cpp/h           # 平台适配层
│   ├── Logger.cpp/h             # 日志系统
│   ├── MemoryChecker.h          # 内存检查辅助
│   ├── PerformanceMonitor.h     # 性能监控辅助
│   ├── UpdateChecker.h          # 更新检查辅助
│   ├── Version.h                # 版本信息
│   └── resource.h               # 资源 ID
├── res/                         # Windows 资源
│   ├── SnipX.rc                 # 资源脚本
│   ├── SnipX.manifest           # Windows 兼容性清单
│   ├── icon.ico                 # 应用图标
│   └── icon_preview.png         # 图标预览
├── docs/                        # 官网/文档站
├── build.bat                    # 本地构建脚本
├── CMakeLists.txt               # CMake 配置
├── optimize.bat                 # 体积优化脚本
├── release.bat                  # 发布构建脚本
├── installer.iss                # Inno Setup 安装脚本
├── TODO.md                      # 任务清单和测试清单
├── USER_MANUAL.md               # 用户手册
├── API_DOCUMENTATION.md         # API 文档
├── DEVELOPER_GUIDE.md           # 开发者指南
└── CHANGELOG.md                 # 更新日志
```

## 文档索引

- [快速开始](QUICKSTART.md)
- [编译说明](COMPILE.md)
- [用户手册](USER_MANUAL.md)
- [API 文档](API_DOCUMENTATION.md)
- [开发者指南](DEVELOPER_GUIDE.md)
- [性能优化指南](PERFORMANCE_OPTIMIZATION.md)
- [更新日志](CHANGELOG.md)
- [任务与测试清单](TODO.md)

## 测试状态

`TODO.md` 中剩余未完成项主要是测试验证类任务：

- 单显示器、多显示器、不同 DPI、不同分辨率、窗口检测测试。
- Windows 7/8/8.1/10/11 兼容性测试。
- 长时间运行、内存泄漏、大量截图、大尺寸截图压力测试。

测试清单和验证进度见 [TODO.md](TODO.md)。

## 体积优化策略

- MSVC：`/O2 /GL /LTCG /OPT:REF /OPT:ICF`。
- MinGW：`-O3 -s -static -mwindows`。
- 减少不必要依赖，优先使用系统 API。
- 使用资源压缩和 UPX 压缩脚本。
- 保持模块边界清晰，便于按需裁剪功能。

## 许可证

本项目基于 [MIT License](LICENSE) 开源。

## 致谢

本项目灵感来源于 X-Snip，目标是提供一个开源、轻量、高性能、可维护的 Windows 截图工具替代方案。
