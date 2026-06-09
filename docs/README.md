# SnipX 文档网站

这是 SnipX 的官方文档网站，使用 VitePress 构建。

## 📦 安装依赖

```bash
npm ci
```

> 首次更新依赖时可运行 `npm install` 并提交生成的 `package-lock.json`；日常构建和 CI 使用 `npm ci`。

## 🚀 本地开发

```bash
npm run docs:dev
```

访问 `http://localhost:5173`

## 🏗️ 构建

```bash
npm run docs:build
```

## 👀 预览

```bash
npm run docs:preview
```

## 📸 静态资源

基础网站资源已放在 `docs/public/`：

- `hero-image.png` - 首页主图
- `logo.png` - 导航栏 Logo
- `favicon.ico` - 网站图标
- `screenshots/` - 后续功能实拍截图目录

如需补充功能截图，请参考 [SCREENSHOTS_NEEDED.md](SCREENSHOTS_NEEDED.md)。

## 📁 当前目录结构

```text
docs/
├── .vitepress/
│   └── config.mts          # VitePress 配置
├── guide/
│   ├── getting-started.md  # 快速开始
│   └── shortcuts.md        # 快捷键
├── public/
│   ├── screenshots/        # 功能截图目录
│   ├── hero-image.png
│   ├── logo.png
│   └── favicon.ico
├── index.md                # 首页
├── features.md             # 功能特性
├── download.md             # 下载页面
├── package.json
├── package-lock.json
└── README.md
```

## 🌍 语言状态

当前文档站以简体中文为主。项目根目录提供英文 README；如果未来要发布英文站点，需要补齐 `docs/en/` 页面并重新启用 VitePress `locales` 配置。

## 🚀 部署到 GitHub Pages

GitHub Actions 配置位于仓库根目录：

```text
.github/workflows/docs.yml
```

推送到 `main` 分支且 `docs/**` 或该 workflow 发生变化时，会自动构建并部署到 GitHub Pages。

部署前请在 GitHub 仓库设置中启用 GitHub Pages：

- Settings → Pages
- Source: GitHub Actions

部署成功后访问：https://vogadero.github.io/SnipX/

## 📝 编写文档

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

### 图片

```markdown
![描述](/screenshots/image.png)
```

在 GitHub Pages 项目页中，VitePress 会根据 `base: '/SnipX/'` 处理 Markdown 资源路径；避免在 raw HTML 中写未带 `/SnipX/` 前缀的站内绝对路径。

## 📞 获取帮助

- [VitePress 文档](https://vitepress.dev/)
- [GitHub Issues](https://github.com/vogadero/SnipX/issues)
- [GitHub Discussions](https://github.com/vogadero/SnipX/discussions)

## 📄 许可证

MIT License
