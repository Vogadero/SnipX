# SnipX v1.0.0 - 首个正式版本 🎉

**发布日期**: 2024-01-15

---

## 🎊 重要里程碑

这是 SnipX 的首个正式版本！经过精心开发和优化，我们很高兴地宣布 SnipX 1.0.0 正式发布。

SnipX 是一款极致轻量的 Windows 截图工具，采用纯 C++ Win32 API 开发，目标体积 < 1MB，启动速度 < 500ms。

---

## ✨ 主要特性

### 🪶 极致轻量
- **体积**: < 1MB（目标）
- **启动**: < 500ms
- **内存**: < 10MB（空闲）
- **CPU**: < 1%（空闲）

### 📸 截图功能
- ✅ **区域截图** - 拖拽选择任意区域
- ✅ **全屏截图** - 一键截取整个屏幕
- ✅ **窗口检测** - 自动检测窗口边界
- ✅ **滚动截图** - 捕获长网页和文档
- ✅ **多显示器** - 完美支持多显示器
- ✅ **放大镜** - 10x 放大辅助
- ✅ **尺寸显示** - 实时显示选区尺寸

### 🎨 标注工具（9种）
- ✅ **矩形** - 绘制矩形边框
- ✅ **椭圆** - 绘制椭圆边框
- ✅ **箭头** - 添加箭头指示
- ✅ **直线** - 绘制直线
- ✅ **铅笔** - 自由绘制
- ✅ **文字** - 添加文本说明
- ✅ **马赛克** - 像素化遮挡
- ✅ **模糊** - 高斯模糊
- ✅ **序号** - 添加序号标签

### 🔧 编辑功能
- ✅ **选中标注** - 点击选中标注对象
- ✅ **拖拽移动** - 拖拽移动标注
- ✅ **调整大小** - 调整标注大小
- ✅ **删除标注** - Delete 键删除
- ✅ **撤销/重做** - Ctrl+Z / Ctrl+Y
- ✅ **颜色选择** - 自定义颜色
- ✅ **粗细调整** - 自定义线条粗细

### 💾 保存功能
- ✅ **PNG 格式** - 无损压缩
- ✅ **JPG 格式** - 可调质量（1-100）
- ✅ **BMP 格式** - 位图格式
- ✅ **自动保存** - 可选自动保存
- ✅ **文件名规则** - 时间戳命名

### 📋 剪贴板
- ✅ **复制** - Ctrl+C 复制到剪贴板
- ✅ **双击复制** - 双击选区复制并退出

### 🎯 高级功能
- ✅ **取色器** - RGB/HSL/HEX 颜色拾取
- ✅ **贴图** - 截图贴回桌面
- ✅ **缩放** - 鼠标滚轮缩放贴图
- ✅ **透明度** - 4 档透明度调节
- ✅ **鼠标穿透** - 贴图鼠标穿透
- ✅ **录屏** - 屏幕录制
- ✅ **GIF 导出** - 导出为 GIF 动画

### ⚙️ 设置功能
- ✅ **启动模式** - 托盘/截图/隐藏
- ✅ **开机自启** - 可选开机自启
- ✅ **托盘图标** - 可显示/隐藏
- ✅ **默认格式** - PNG/JPG/BMP
- ✅ **保存路径** - 自定义保存路径
- ✅ **JPG 质量** - 可调质量
- ✅ **文件名前缀** - 自定义前缀
- ✅ **默认颜色** - 自定义默认颜色
- ✅ **默认粗细** - 自定义默认粗细

### ⌨️ 快捷键
- `F1` / `Ctrl+Alt+A` - 启动截图
- `Esc` - 取消/关闭
- `Enter` - 确认选区
- `Ctrl+S` - 保存
- `Ctrl+C` - 复制
- `Ctrl+Z` - 撤销
- `Ctrl+Y` - 重做
- `Delete` - 删除选中标注
- `方向键` - 微调选区（1px）
- `Shift+方向键` - 微调选区（10px）

### 🌍 多语言
- ✅ 中文支持
- ✅ 英文支持
- ✅ 可扩展其他语言

---

## 📦 下载

### 推荐下载

- **单文件版本**: `SnipX.exe` (推荐)
  - 无需安装，解压即用
  - 体积小，启动快

- **安装程序**: `SnipX-1.0.0-Setup.exe`
  - 完整的安装向导
  - 可选开机自启
  - 可选桌面快捷方式

- **完整包**: `SnipX-1.0.0.zip`
  - 包含程序和文档
  - 适合离线使用

### 文件说明

| 文件 | 大小 | 说明 |
|------|------|------|
| SnipX.exe | < 1MB | 单文件版本 |
| SnipX-1.0.0-Setup.exe | ~1.5MB | 安装程序 |
| SnipX-1.0.0.zip | ~2MB | 完整包 |

---

## 📋 系统要求

- **操作系统**: Windows 7 或更高版本
- **架构**: x64
- **依赖**: 无（使用系统自带的 GDI+）
- **磁盘空间**: < 5MB
- **内存**: 建议 2GB 或以上

---

## 🚀 快速开始

### 使用单文件版本

1. 下载 `SnipX.exe`
2. 双击运行
3. 按 `F1` 或 `Ctrl+Alt+A` 开始截图

### 使用安装程序

1. 下载 `SnipX-1.0.0-Setup.exe`
2. 运行安装程序
3. 按照向导完成安装
4. 启动 SnipX
5. 按 `F1` 或 `Ctrl+Alt+A` 开始截图

---

## 📖 文档

### 用户文档
- [用户手册](https://github.com/vogadero/SnipX/blob/main/USER_MANUAL.md) - 详细使用说明
- [快速开始](https://github.com/vogadero/SnipX/blob/main/QUICKSTART.md) - 5 分钟上手
- [更新日志](https://github.com/vogadero/SnipX/blob/main/CHANGELOG.md) - 版本历史

### 开发文档
- [开发者指南](https://github.com/vogadero/SnipX/blob/main/DEVELOPER_GUIDE.md) - 开发指南
- [编译指南](https://github.com/vogadero/SnipX/blob/main/COMPILE.md) - 编译说明
- [API 文档](https://github.com/vogadero/SnipX/blob/main/API_DOCUMENTATION.md) - API 参考

---

## 🐛 已知问题

1. **高 DPI 适配** - 需要在不同 DPI 下进一步测试
2. **Win7 兼容性** - 需要在 Win7 上验证
3. **自动保存重启生效** - 自动保存设置需要重启才能生效

这些问题将在后续版本中修复。

---

## 🔄 更新说明

### 如何更新

1. **手动更新**:
   - 下载新版本
   - 替换旧版本文件
   - 配置文件会自动保留

2. **自动更新**（未来版本）:
   - 程序会自动检查更新
   - 提示下载新版本

---

## 🤝 贡献

欢迎贡献代码、报告问题或提出建议！

- **报告问题**: [GitHub Issues](https://github.com/vogadero/SnipX/issues)
- **贡献代码**: [贡献指南](https://github.com/vogadero/SnipX/blob/main/CONTRIBUTING.md)
- **讨论交流**: [GitHub Discussions](https://github.com/vogadero/SnipX/discussions)

---

## 🔒 安全

如果发现安全漏洞，请查看 [安全策略](https://github.com/vogadero/SnipX/blob/main/SECURITY.md) 了解报告流程。

---

## 📄 许可证

SnipX 采用 [MIT 许可证](https://github.com/vogadero/SnipX/blob/main/LICENSE.txt)。

---

## 🙏 致谢

感谢所有测试人员、反馈用户和贡献者！

特别感谢：
- 所有提供反馈的用户
- 所有测试人员
- 所有贡献者

---

## 📞 联系方式

- **GitHub**: https://github.com/vogadero/SnipX
- **Issues**: https://github.com/vogadero/SnipX/issues
- **Discussions**: https://github.com/vogadero/SnipX/discussions

---

## 🎯 路线图

### v1.1.0（计划中）
- 修复已知问题
- 性能优化
- 用户反馈改进

### v2.0.0（未来）
- 插件系统
- 云同步
- 跨平台支持

---

**SnipX - 让截图回归简单与轻量** 🚀

**Made with ❤️ by SnipX Team**

---

**完整更新日志**: [CHANGELOG.md](https://github.com/vogadero/SnipX/blob/main/CHANGELOG.md)

**项目主页**: https://github.com/vogadero/SnipX
