# 下载

## 📦 最新版本

### v1.0.0（2024-01-15）

这是 SnipX 的首个正式版本。

---

## 💾 下载选项

### 🚀 单文件版本（推荐）

**SnipX.exe**

- 体积目标：< 1MB
- 无需安装，下载即用
- 适合便携使用

<a href="https://github.com/vogadero/SnipX/releases/download/v1.0.0/SnipX.exe" class="download-button">下载 SnipX.exe</a>

---

### 📁 完整包

**SnipX-1.0.0.zip**

- 包含程序和离线文档
- 适合归档、离线使用和分发

<a href="https://github.com/vogadero/SnipX/releases/download/v1.0.0/SnipX-1.0.0.zip" class="download-button">下载完整包</a>

---

### 🔒 校验文件

**SnipX-1.0.0-SHA256.txt**

- 包含 Release 资产的 SHA256 校验和
- 用于验证下载文件完整性

<a href="https://github.com/vogadero/SnipX/releases/download/v1.0.0/SnipX-1.0.0-SHA256.txt" class="download-button">下载 SHA256 校验文件</a>

::: tip 安装版说明
仓库提供了 `installer.iss` 作为 Inno Setup 安装脚本，便于后续制作安装包；当前 GitHub Release workflow 上传的是便携 exe、zip 和 SHA256 文件。
:::

---

## 📋 系统要求

### 最低要求

- **操作系统**：Windows 7 或更高版本
- **架构**：x86 或 x64，推荐 x64
- **内存**：2GB RAM
- **磁盘空间**：< 5MB（不含截图存储）

### 推荐配置

- **操作系统**：Windows 10/11
- **架构**：x64
- **内存**：4GB RAM 或以上
- **磁盘空间**：10MB（包含截图存储）

### 依赖项

- ✅ 使用系统自带 Win32 API、GDI+ 和 Common Controls
- ✅ 便携版无需额外安装运行时
- ✅ 纯绿色软件

---

## 🚀 使用指南

### 使用单文件版本

1. **下载** `SnipX.exe`
2. **运行** 双击 `SnipX.exe`
3. **开始使用** 按 `Alt+X` 开始区域截图，或按 `Alt+F` 进行全屏截图

::: tip 提示
单文件版本无需安装，可以放在任意位置运行。建议放在固定文件夹中，方便管理程序和更新。
:::

---

### 使用完整包

1. **下载** `SnipX-1.0.0.zip`
2. **解压** 到任意位置
3. **运行** 双击 `SnipX.exe`
4. **查看文档** 阅读随包文档了解更多功能

---

## 🔄 更新说明

### 手动更新

1. 下载最新版本
2. 关闭正在运行的 SnipX
3. 替换旧版本文件
4. 重新启动 SnipX

::: warning 注意
配置文件会自动保留，无需备份。配置文件位置：`%APPDATA%\SnipX\config.ini`。
:::

---

## 📝 版本历史

### v1.0.0（2024-01-15）

**首个正式版本** 🎉

**新功能**：
- ✅ 完整的截图功能（区域/全屏/窗口/滚动）
- ✅ 10 种标注工具
- ✅ 完整的编辑功能
- ✅ 多格式保存（PNG/JPG/BMP）
- ✅ 取色器功能
- ✅ 贴图功能
- ✅ 录屏帧序列功能
- ✅ 多语言支持

**性能**：
- ✅ 体积目标 < 1MB
- ✅ 启动目标 < 500ms
- ✅ 空闲内存目标 < 10MB
- ✅ 空闲 CPU 目标 < 1%

查看完整的 [更新日志](https://github.com/vogadero/SnipX/blob/main/CHANGELOG.md)。

---

## 🔒 安全说明

### 文件校验

请下载同版本的 `SnipX-1.0.0-SHA256.txt`，并将其中的哈希值与本地文件计算结果比对。

**Windows PowerShell**：

```powershell
Get-FileHash SnipX.exe -Algorithm SHA256
Get-FileHash SnipX-1.0.0.zip -Algorithm SHA256
```

**命令提示符**：

```cmd
certutil -hashfile SnipX.exe SHA256
certutil -hashfile SnipX-1.0.0.zip SHA256
```

---

## 🆘 下载问题

### 下载速度慢

如果 GitHub 下载速度慢，可以尝试：

1. 刷新页面重试
2. 更换浏览器
3. 检查网络连接
4. 使用可信网络环境重新下载

### 文件损坏

如果下载的文件无法运行：

1. 重新下载
2. 验证 SHA256 校验和
3. 检查杀毒软件是否误报

---

## 🛡️ 安全提示

### 官方下载渠道

请从以下官方渠道下载 SnipX：

- ✅ **GitHub Releases**：https://github.com/vogadero/SnipX/releases
- ✅ **官方网站**：https://vogadero.github.io/SnipX/

::: danger 警告
请勿从非官方渠道下载 SnipX，以免下载到被篡改的版本。
:::

---

## 🔧 从源代码编译

如果你想从源代码编译 SnipX：

1. **克隆仓库**
   ```bash
   git clone https://github.com/vogadero/SnipX.git
   cd SnipX
   ```

2. **编译**
   ```cmd
   build.bat
   ```

3. **运行**
   ```cmd
   bin\SnipX.exe
   ```

查看详细的 [编译指南](https://github.com/vogadero/SnipX/blob/main/COMPILE.md)。

---

## 📞 获取帮助

如果在下载或使用过程中遇到问题：

- 📖 查看 [快速开始](/guide/getting-started)
- 🐛 [报告问题](https://github.com/vogadero/SnipX/issues)
- 💬 [讨论交流](https://github.com/vogadero/SnipX/discussions)

---

<div style="text-align: center; margin-top: 40px;">
  <h2>立即下载 SnipX</h2>
  <p style="font-size: 18px; color: #666;">让截图回归简单与轻量</p>
  <a href="https://github.com/vogadero/SnipX/releases/latest" style="display: inline-block; padding: 12px 32px; background: #3eaf7c; color: white; border-radius: 4px; text-decoration: none; font-weight: 600; margin-top: 20px;">前往 GitHub Releases</a>
</div>

<style>
.download-button {
  display: inline-block;
  padding: 10px 24px;
  background: #3eaf7c;
  color: white !important;
  border-radius: 4px;
  text-decoration: none;
  font-weight: 600;
  margin-top: 10px;
  transition: background 0.3s;
}

.download-button:hover {
  background: #2d8f5f;
}
</style>
