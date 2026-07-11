# SnipX v1.1.7 - 文档注释与结构清理

**发布日期**：2026-07-11

**适用平台**：Windows 7 / 8 / 8.1 / 10 / 11，推荐 Windows 10/11 x64

**许可协议**：MIT License

SnipX v1.1.7 聚焦代码可维护性：为关键模块补齐 JSDoc 风格契约注释，并做不改变业务行为的轻量封装清理。

---

## 下载

| 文件 | 适用场景 | 说明 |
| --- | --- | --- |
| `SnipX.exe` | 便携版 | 下载后直接运行，无需安装 |
| `SnipX-1.1.7.zip` | 完整包 | 包含程序、README、用户手册和更新日志 |
| `SnipX-1.1.7-SHA256.txt` | 校验文件 | 用于校验发布资产完整性 |

---

## 本版本重点更新

### 文档与注释

- 为 Config、SnipX、Editor、ScreenCapture、ColorPicker、PinWindow、Tray/Hotkey、Settings、Recorder 等核心头文件补齐 `/** ... */` 文档注释。
- 在配置加载、热键分发、录屏启停、截图所有权转移等关键路径补充中文 `//` 说明。

### 结构清理

- Config 写出 INI 时复用 `WriteIniInt` / `WriteIniBool`。
- 截图选区完成统一走 `ClipRectToVirtualScreen` + `CompleteSelection`。
- 删除无调用的 `ConfirmSelection` 重复裁剪实现。
- 编辑器底部 6 个按钮的绘制、hover、tooltip 使用统一数量常量，并补齐 OCR/上传提示文案。

---

## 快速开始

1. 从本 Release 下载 `SnipX.exe` 或 `SnipX-1.1.7.zip`。
2. 如果下载的是 ZIP 包，先解压到任意目录。
3. 双击运行 `SnipX.exe`。
4. 程序启动后驻留系统托盘。
5. 按 `Alt + X` 开始区域截图。

---

## 已知限制

- 滚动截图当前仍是简化实现。
- 录屏当前以 PNG 帧序列为主，GIF 导出为基础能力。
- OCR 和上传入口仍为可扩展占位功能。

---

## 校验

```powershell
Get-FileHash .\SnipX.exe -Algorithm SHA256
Get-FileHash .\SnipX-1.1.7.zip -Algorithm SHA256
```

将输出值与 `SnipX-1.1.7-SHA256.txt` 中的 SHA256 值比对即可。

---

## 完整变更

- [CHANGELOG.md](https://github.com/Vogadero/SnipX/blob/main/CHANGELOG.md)
- [v1.1.7 提交历史](https://github.com/Vogadero/SnipX/commits/v1.1.7)
