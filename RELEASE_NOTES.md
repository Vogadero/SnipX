# SnipX v1.1.6 - 编辑器重绘清理优化

**发布日期**：2026-07-11

**适用平台**：Windows 7 / 8 / 8.1 / 10 / 11，推荐 Windows 10/11 x64

**许可协议**：MIT License

SnipX v1.1.6 是 v1.1.5 编辑器性能优化后的清理发布，聚焦降低截图编辑器拖拽、缩放和绘制过程中的无效刷新，并整理相关内部接口，便于后续继续维护。

---

## 下载

| 文件 | 适用场景 | 说明 |
| --- | --- | --- |
| `SnipX.exe` | 便携版 | 下载后直接运行，无需安装 |
| `SnipX-1.1.6.zip` | 完整包 | 包含程序、README、用户手册和更新日志 |
| `SnipX-1.1.6-SHA256.txt` | 校验文件 | 用于校验发布资产完整性 |

---

## 本版本重点更新

### 编辑器重绘清理

- 移动、缩放和绘制标注时，若鼠标位置或标注边界没有实际变化，会跳过后续局部刷新。
- 复用统一的图片拖拽矩形计算逻辑，减少矩形、椭圆、高亮、模糊和马赛克标注中的重复代码。
- 模糊和马赛克标注在绘制阶段更新区域时同步标记缓存失效，避免缓存状态和当前区域维护分散。

### 维护性改进

- `UpdateAnnotation`、`MoveSelectedAnnotation`、`ResizeSelectedAnnotation` 现在通过返回值明确表示是否发生实际变化。
- 补充相关接口注释，说明返回值语义，降低后续维护成本。

---

## 快速开始

1. 从本 Release 下载 `SnipX.exe` 或 `SnipX-1.1.6.zip`。
2. 如果下载的是 ZIP 包，先解压到任意目录。
3. 双击运行 `SnipX.exe`。
4. 程序启动后驻留系统托盘。
5. 按 `Alt + X` 开始区域截图，或右键托盘图标使用滚动截图、录屏、设置等功能。

---

## 已知限制

- 滚动截图当前仍是简化实现，复杂应用、动态网页或非标准滚动控件可能无法完整拼接。
- 录屏当前以 PNG 帧序列为主，GIF 导出为基础占位能力。
- OCR 和上传入口仍为可扩展占位功能。
- 高 DPI、多显示器不同 DPI 的细节仍建议在真实设备上持续验证。

---

## 校验

Release 工作流会生成 `SnipX-1.1.6-SHA256.txt`。下载后可在 PowerShell 中使用：

```powershell
Get-FileHash .\SnipX.exe -Algorithm SHA256
Get-FileHash .\SnipX-1.1.6.zip -Algorithm SHA256
```

将输出值与 `SnipX-1.1.6-SHA256.txt` 中的 SHA256 值比对即可。

---

## 完整变更

- [CHANGELOG.md](https://github.com/Vogadero/SnipX/blob/main/CHANGELOG.md)
- [v1.1.6 提交历史](https://github.com/Vogadero/SnipX/commits/v1.1.6)
