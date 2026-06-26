# SnipX v1.1.4 - 编辑器界面优化

**发布日期**：2026-06-26

**适用平台**：Windows 7 / 8 / 8.1 / 10 / 11，推荐 Windows 10/11 x64

**许可协议**：MIT License

SnipX 是一款基于纯 Win32 C++17、GDI+ 和系统 API 开发的轻量级 Windows 截图工具，聚焦快速截图、标注、贴图、取色、录屏与便携发布。v1.1.4 聚焦截图编辑器界面体验，对工具栏和底部操作栏的配色、边框、字体和交互状态做了统一优化，让整体观感更清爽、更一致。

---

## 下载

| 文件 | 适用场景 | 说明 |
| --- | --- | --- |
| `SnipX.exe` | 便携版 | 下载后直接运行，无需安装 |
| `SnipX-1.1.4.zip` | 完整包 | 包含程序、README、用户手册和更新日志 |
| `SnipX-1.1.4-SHA256.txt` | 校验文件 | 用于校验发布资产完整性 |

> 当前 Release 工作流发布便携版和 ZIP 包；安装器脚本已在仓库中提供，但暂未作为正式 Release 资产上传。

---

## 本版本重点更新

### 编辑器界面优化

- 优化截图编辑器顶部工具栏配色，使用更清爽的浅色表面和更柔和的边框。
- 优化底部操作栏按钮样式，统一文字颜色、悬停背景和高亮边框。
- 将工具栏和底栏字体统一为 `Microsoft YaHei UI`，提升 Windows 桌面环境下的 UI 一致性。
- 优化工具按钮 hover / selected 状态，让当前工具和鼠标悬停反馈更清晰。

---

## 主要特性

### 截图

- 区域截图：拖拽选择任意屏幕区域。
- 全屏截图：快速截取虚拟桌面完整画面。
- 窗口检测：自动识别窗口边界。
- 滚动截图：提供简化长截图流程。
- 多显示器：支持虚拟桌面坐标和多屏截图场景。
- 上次选区：可复用最近一次截图区域。

### 标注与编辑

- 矩形、椭圆、箭头、直线、铅笔、文字。
- 马赛克、模糊、高亮、序号标签。
- 标注对象选择、移动、删除、撤销/重做。
- 支持数字键快速切换标注工具。
- 工具栏、颜色选择器、粗细选择器和悬停提示。
- 保存为 PNG/JPG/BMP，支持 JPG 质量配置。
- 复制到剪贴板，支持双击复制并退出。

### 贴图、取色与录屏

- 取色器：支持 HEX/RGB/HSL/HSV 显示与复制。
- 贴图：截图贴回桌面，支持拖拽、缩放、透明度调节和鼠标穿透。
- 录屏：支持屏幕帧序列录制，并提供基础 GIF 导出入口。

---

## 默认快捷键

| 快捷键 | 功能 |
| --- | --- |
| `Alt + X` | 区域截图 |
| `Alt + F` | 全屏截图 |
| `Shift + Alt + X` | 复用上次选区 |
| `Esc` | 取消截图、关闭编辑器或关闭贴图窗口 |
| `Enter` | 确认截图选区 |
| `Ctrl + S` | 保存截图 / 保存贴图 |
| `Ctrl + C` | 复制截图 / 复制贴图 |
| `Ctrl + Z / Ctrl + Y` | 撤销 / 重做 |
| `Delete / Backspace` | 删除选中标注 |
| `1`-`9`、`0` | 编辑器中快速切换标注工具 |
| `R / H` | 取色器显示模式切换；截图过程中进入取色器 |
| `C` | 取色器复制当前显示模式的颜色值 |
| `1 / 2 / 3 / 4` | 取色器复制 HEX / RGB / HSL / HSV |

---

## 快速开始

1. 从本 Release 下载 `SnipX.exe` 或 `SnipX-1.1.4.zip`。
2. 如果下载的是 ZIP 包，先解压到任意目录。
3. 双击运行 `SnipX.exe`。
4. 程序启动后驻留系统托盘。
5. 按 `Alt + X` 开始区域截图，或按 `Alt + F` 进行全屏截图。
6. 截图后可进行标注、保存、复制、贴图、取色等操作。

首次运行会在 `%APPDATA%\SnipX` 下创建配置和日志相关文件。

---

## 已知限制

- 高 DPI、多显示器不同 DPI、Windows 7/8.1 兼容性仍建议在真实设备上继续验证。
- OCR 当前为可扩展占位实现，不是完整 OCR 引擎。
- 图片上传当前为本地导出待上传图片的占位入口，尚未接入真实图床服务。
- GIF 导出当前为基础导出入口，录屏主能力更准确地说是屏幕帧序列录制。

---

## 文档

- [README](https://github.com/Vogadero/SnipX/blob/main/README.md)
- [用户手册](https://github.com/Vogadero/SnipX/blob/main/USER_MANUAL.md)
- [快速开始](https://github.com/Vogadero/SnipX/blob/main/QUICKSTART.md)
- [更新日志](https://github.com/Vogadero/SnipX/blob/main/CHANGELOG.md)
- [编译指南](https://github.com/Vogadero/SnipX/blob/main/COMPILE.md)

---

## 校验

Release 工作流会生成 `SnipX-1.1.4-SHA256.txt`。下载后可在 PowerShell 中使用：

```powershell
Get-FileHash .\SnipX.exe -Algorithm SHA256
Get-FileHash .\SnipX-1.1.4.zip -Algorithm SHA256
```

将输出值与 `SnipX-1.1.4-SHA256.txt` 中的 SHA256 值比对即可。

---

## 完整变更

- [CHANGELOG.md](https://github.com/Vogadero/SnipX/blob/main/CHANGELOG.md)
- [v1.1.4 提交历史](https://github.com/Vogadero/SnipX/commits/v1.1.4)
