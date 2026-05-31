# 📸 SnipX 网站截图需求清单

## 🎯 截图目的

这些截图将用于 SnipX 官方文档网站（https://vogadero.github.io/SnipX/），帮助用户直观了解 SnipX 的功能和特性。

---

## 📋 必需截图列表

### 1. 核心截图（必需）

#### 1.1 hero-image.png
- **用途**：首页主图（Hero 区域）
- **推荐尺寸**：800x600 或 1200x900
- **内容要求**：
  - SnipX 主界面或功能展示
  - 展示截图和标注功能
  - 背景干净整洁
  - 突出 SnipX 的轻量和强大
- **示例场景**：
  - 正在进行区域截图
  - 显示工具栏和标注工具
  - 展示放大镜和尺寸信息
- **存放位置**：`docs/public/hero-image.png`

#### 1.2 logo.png
- **用途**：网站 Logo（导航栏）
- **推荐尺寸**：128x128 或 256x256
- **内容要求**：
  - SnipX Logo
  - 透明背景（PNG）
  - 清晰锐利
- **存放位置**：`docs/public/logo.png`

#### 1.3 favicon.ico
- **用途**：浏览器标签页图标
- **尺寸**：16x16, 32x32, 48x48（多尺寸 ICO）
- **内容要求**：
  - SnipX 图标
  - 简洁清晰
- **存放位置**：`docs/public/favicon.ico`

---

### 2. 功能截图（推荐）

#### 2.1 main-interface.png
- **用途**：展示主界面
- **推荐尺寸**：1920x1080
- **内容要求**：
  - SnipX 主窗口
  - 托盘图标
  - 设置界面（可选）
- **示例场景**：
  - 程序启动后的状态
  - 托盘图标右键菜单
- **存放位置**：`docs/public/screenshots/main-interface.png`

#### 2.2 region-capture.png
- **用途**：展示区域截图功能
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 正在选择截图区域
  - 显示选区尺寸信息
  - 显示放大镜
  - 显示鼠标坐标
- **示例场景**：
  - 拖拽选择区域
  - 显示实时尺寸
- **存放位置**：`docs/public/screenshots/region-capture.png`

#### 2.3 window-detection.png
- **用途**：展示窗口检测功能
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 窗口自动高亮显示
  - 显示窗口边界
  - 多个窗口场景
- **示例场景**：
  - 鼠标移动到窗口上
  - 窗口自动高亮
- **存放位置**：`docs/public/screenshots/window-detection.png`

#### 2.4 annotation-tools.png
- **用途**：展示标注工具
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 显示工具栏
  - 展示多种标注工具的效果
  - 矩形、椭圆、箭头、文字等
  - 不同颜色和粗细
- **示例场景**：
  - 在截图上添加各种标注
  - 展示标注工具的多样性
- **存放位置**：`docs/public/screenshots/annotation-tools.png`

#### 2.5 color-picker.png
- **用途**：展示取色器功能
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 取色器界面
  - 显示 RGB/HSL/HEX 值
  - 显示 10x 放大镜
  - 显示颜色预览
- **示例场景**：
  - 正在取色
  - 显示颜色信息
- **存放位置**：`docs/public/screenshots/color-picker.png`

#### 2.6 pin-window.png
- **用途**：展示贴图功能
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 贴图窗口
  - 显示贴图控制按钮
  - 展示透明度效果
  - 展示缩放效果
- **示例场景**：
  - 截图贴回桌面
  - 显示贴图窗口
- **存放位置**：`docs/public/screenshots/pin-window.png`

#### 2.7 settings.png
- **用途**：展示设置界面
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 设置对话框
  - 显示各个设置选项
  - 基本设置、保存设置、标注设置
- **示例场景**：
  - 打开设置界面
  - 展示设置选项
- **存放位置**：`docs/public/screenshots/settings.png`

#### 2.8 recording.png
- **用途**：展示录屏功能
- **推荐尺寸**：1920x1080
- **内容要求**：
  - 录屏界面
  - 显示录制控制按钮
  - 显示录制区域
- **示例场景**：
  - 正在录屏
  - 显示录制状态
- **存放位置**：`docs/public/screenshots/recording.png`

---

## 📐 截图规范

### 尺寸要求

| 类型 | 推荐尺寸 | 最小尺寸 |
|------|----------|----------|
| Hero 图 | 1200x900 | 800x600 |
| Logo | 256x256 | 128x128 |
| Favicon | 48x48 | 16x16 |
| 功能截图 | 1920x1080 | 1280x720 |

### 格式要求

- **格式**：PNG（推荐）或 JPG
- **透明背景**：Logo 和 Favicon 需要透明背景
- **压缩**：适当压缩以减小文件大小（< 500KB）

### 内容要求

- **清晰度**：高清晰度，文字清晰可读
- **背景**：干净整洁，不要有杂乱内容
- **焦点**：突出功能特点，避免干扰元素
- **真实性**：使用真实的 SnipX 界面，不要 PS 或修改
- **一致性**：所有截图风格保持一致

### 命名规范

- 使用小写字母
- 使用连字符分隔单词
- 使用描述性名称
- 例如：`region-capture.png`、`color-picker.png`

---

## 🎨 截图技巧

### 1. 使用 SnipX 自身截图

最好的方式是使用 SnipX 自己来截图，这样可以展示真实的使用场景。

### 2. 准备干净的背景

- 关闭不必要的窗口
- 清理桌面图标
- 使用纯色或简洁的壁纸

### 3. 展示关键功能

- 突出显示正在使用的功能
- 使用箭头或高亮标注关键区域
- 确保功能特点清晰可见

### 4. 保持一致性

- 使用相同的窗口大小
- 使用相同的颜色方案
- 使用相同的字体大小

### 5. 优化文件大小

- 使用 PNG 格式（支持透明）
- 适当压缩（TinyPNG、ImageOptim）
- 保持清晰度的同时减小文件大小

---

## 📁 目录结构

```
docs/
└── public/
    ├── hero-image.png          # 首页主图
    ├── logo.png                # Logo
    ├── favicon.ico             # 网站图标
    └── screenshots/            # 功能截图目录
        ├── main-interface.png
        ├── region-capture.png
        ├── window-detection.png
        ├── annotation-tools.png
        ├── color-picker.png
        ├── pin-window.png
        ├── settings.png
        └── recording.png
```

---

## ✅ 截图检查清单

### 准备阶段

- [ ] 安装并运行 SnipX
- [ ] 准备干净的桌面背景
- [ ] 关闭不必要的窗口
- [ ] 准备截图工具（可以用 SnipX 自己）

### 截图阶段

- [ ] hero-image.png（首页主图）
- [ ] logo.png（Logo）
- [ ] favicon.ico（网站图标）
- [ ] main-interface.png（主界面）
- [ ] region-capture.png（区域截图）
- [ ] window-detection.png（窗口检测）
- [ ] annotation-tools.png（标注工具）
- [ ] color-picker.png（取色器）
- [ ] pin-window.png（贴图功能）
- [ ] settings.png（设置界面）
- [ ] recording.png（录屏功能）

### 后期处理

- [ ] 检查清晰度
- [ ] 检查文件大小
- [ ] 优化压缩
- [ ] 检查命名
- [ ] 放置到正确目录

### 测试验证

- [ ] 在网站中预览
- [ ] 检查显示效果
- [ ] 检查加载速度
- [ ] 检查响应式显示

---

## 🔧 工具推荐

### 截图工具

- **SnipX**（推荐）- 使用自己截图
- **Snipping Tool** - Windows 自带
- **ShareX** - 开源截图工具

### 图片编辑

- **Paint.NET** - 免费图片编辑
- **GIMP** - 开源图片编辑
- **Photoshop** - 专业图片编辑

### 图片压缩

- **TinyPNG** - https://tinypng.com/
- **ImageOptim** - https://imageoptim.com/
- **Squoosh** - https://squoosh.app/

### 图标制作

- **IcoFX** - ICO 图标制作
- **RealWorld Icon Editor** - 免费图标编辑
- **Online ICO Converter** - https://www.icoconverter.com/

---

## 📞 需要帮助？

如果在准备截图时遇到问题：

- 📖 查看 [WEBSITE_DEPLOYMENT_GUIDE.md](../WEBSITE_DEPLOYMENT_GUIDE.md)
- 💬 在 [GitHub Discussions](https://github.com/vogadero/SnipX/discussions) 讨论
- 🐛 在 [GitHub Issues](https://github.com/vogadero/SnipX/issues) 报告问题

---

## 📝 提交截图

### 方式 1：直接提交

1. 将截图放到对应目录
2. 提交到 Git：
   ```cmd
   git add docs/public/
   git commit -m "Add website screenshots"
   git push
   ```

### 方式 2：Pull Request

1. Fork 仓库
2. 添加截图
3. 提交 Pull Request

---

## 🎉 感谢

感谢你为 SnipX 文档网站贡献截图！

你的贡献将帮助更多用户了解和使用 SnipX。

---

**SnipX - 让截图回归简单与轻量** 🚀

**最后更新**：2024-01-15
