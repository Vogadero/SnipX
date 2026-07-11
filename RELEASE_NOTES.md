# SnipX v1.1.8 - 公共工具封装

**发布日期**：2026-07-12

**适用平台**：Windows 7 / 8 / 8.1 / 10 / 11，推荐 Windows 10/11 x64

**许可协议**：MIT License

SnipX v1.1.8 在文档清理基础上继续做结构封装：抽出跨模块重复的 GDI+/Win32 工具，并补齐关键热路径注释。业务行为保持不变。

---

## 下载

| 文件 | 适用场景 | 说明 |
| --- | --- | --- |
| `SnipX.exe` | 便携版 | 下载后直接运行，无需安装 |
| `SnipX-1.1.8.zip` | 完整包 | 包含程序、README、用户手册和更新日志 |
| `SnipX-1.1.8-SHA256.txt` | 校验文件 | 用于校验发布资产完整性 |

---

## 本版本重点更新

### 公共工具封装

- 新增 `GdiplusUtils`：
  - `GetEncoderClsidByMime`
  - `CaptureVirtualScreenBitmap`
  - `DrawMagnifierOverlay`
  - `SetClipboardBitmap` / `SetClipboardUnicodeText`
- 截图、录屏、取色、编辑器保存/复制、贴图复制统一复用上述实现。

### 交互一致性

- 编辑器底部栏 hover 直接复用 `HitTestBottomBar`，与点击命中保持同一布局源。

### 注释

- 补充版本宏文档与截图/录屏关键路径中文说明。

---

## 快速开始

1. 从本 Release 下载 `SnipX.exe` 或 `SnipX-1.1.8.zip`。
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
Get-FileHash .\SnipX-1.1.8.zip -Algorithm SHA256
```

将输出值与 `SnipX-1.1.8-SHA256.txt` 中的 SHA256 值比对即可。

---

## 完整变更

- [CHANGELOG.md](https://github.com/Vogadero/SnipX/blob/main/CHANGELOG.md)
- [v1.1.8 提交历史](https://github.com/Vogadero/SnipX/commits/v1.1.8)
