# SnipX 文档网站

这是 SnipX 的官方文档网站，使用 VitePress 构建。

## 📦 安装依赖

```bash
npm install
```

## 🚀 本地开发

```bash
npm run docs:dev
```

访问 http://localhost:5173

## 🏗️ 构建

```bash
npm run docs:build
```

## 👀 预览

```bash
npm run docs:preview
```

## 📸 截图要求

请将以下截图放置在 `/docs/public/screenshots/` 目录：

### 必需截图

1. **hero-image.png** - 首页主图（推荐尺寸：800x600）
2. **logo.png** - Logo（推荐尺寸：128x128）
3. **favicon.ico** - 网站图标（16x16, 32x32, 48x48）

### 功能截图

4. **main-interface.png** - 主界面
5. **region-capture.png** - 区域截图
6. **window-detection.png** - 窗口检测
7. **annotation-tools.png** - 标注工具
8. **color-picker.png** - 取色器
9. **pin-window.png** - 贴图功能
10. **settings.png** - 设置界面
11. **recording.png** - 录屏功能

### 截图规范

- 格式：PNG
- 分辨率：1920x1080 或更高
- 背景：干净整洁
- 内容：突出功能特点
- 文字：清晰可读

## 📁 目录结构

```
docs/
├── .vitepress/
│   ├── config.mts          # VitePress 配置
│   └── dist/               # 构建输出
├── .github/
│   └── workflows/
│       └── deploy.yml      # GitHub Pages 部署
├── guide/                  # 指南文档
│   ├── getting-started.md
│   ├── basic-features.md
│   ├── annotation-tools.md
│   ├── advanced-features.md
│   ├── shortcuts.md
│   ├── settings.md
│   ├── compile.md
│   ├── contributing.md
│   └── api.md
├── public/                 # 静态资源
│   ├── screenshots/        # 截图
│   ├── logo.png
│   └── favicon.ico
├── index.md                # 首页
├── features.md             # 功能特性
├── download.md             # 下载页面
├── package.json
└── README.md               # 本文件
```

## 🌍 多语言支持

网站支持中英文双语：

- 中文：`/`
- 英文：`/en/`

添加新语言：

1. 在 `.vitepress/config.mts` 中添加语言配置
2. 创建对应的语言目录和文件

## 🚀 部署到 GitHub Pages

### 自动部署

推送到 `main` 分支后，GitHub Actions 会自动构建并部署到 GitHub Pages。

### 手动部署

1. 构建网站：
   ```bash
   npm run docs:build
   ```

2. 推送到 GitHub：
   ```bash
   git add .
   git commit -m "Update docs"
   git push
   ```

3. 在 GitHub 仓库设置中启用 GitHub Pages：
   - Settings → Pages
   - Source: GitHub Actions

### 访问网站

部署成功后，访问：https://vogadero.github.io/SnipX/

## 📝 编写文档

### Markdown 语法

VitePress 支持标准 Markdown 语法和扩展语法：

- [VitePress Markdown](https://vitepress.dev/guide/markdown)
- [Markdown 扩展](https://vitepress.dev/guide/markdown#markdown-extensions)

### 自定义容器

```markdown
::: tip 提示
这是一个提示
:::

::: warning 警告
这是一个警告
:::

::: danger 危险
这是一个危险提示
:::
```

### 代码块

````markdown
```javascript
console.log('Hello, SnipX!')
```
````

### 图片

```markdown
![描述](/screenshots/image.png)
```

## 🎨 自定义样式

在 `.vitepress/theme/custom.css` 中添加自定义样式。

## 🔧 配置

在 `.vitepress/config.mts` 中修改配置：

- 网站标题和描述
- 导航栏
- 侧边栏
- 主题配置
- 多语言配置

## 📞 获取帮助

- [VitePress 文档](https://vitepress.dev/)
- [GitHub Issues](https://github.com/vogadero/SnipX/issues)
- [GitHub Discussions](https://github.com/vogadero/SnipX/discussions)

## 📄 许可证

MIT License
