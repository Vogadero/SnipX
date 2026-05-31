# SnipX 网站部署指南

本指南将帮助你部署 SnipX 文档网站到 GitHub Pages。

## 📋 准备工作

### 1. 环境要求

- ✅ Node.js 18+ 已安装
- ✅ npm 或 yarn 已安装
- ✅ Git 已安装
- ✅ GitHub 账号

### 2. 检查环境

```cmd
node --version
npm --version
git --version
```

---

## 📸 准备截图

### 必需截图列表

在 `docs/public/screenshots/` 目录中放置以下截图：

#### 核心截图（必需）

1. **hero-image.png** - 首页主图
   - 推荐尺寸：800x600
   - 内容：SnipX 主界面或功能展示
   - 用途：首页 Hero 区域

2. **logo.png** - Logo
   - 推荐尺寸：128x128
   - 内容：SnipX Logo
   - 用途：导航栏和页面标题

3. **favicon.ico** - 网站图标
   - 尺寸：16x16, 32x32, 48x48
   - 内容：SnipX 图标
   - 用途：浏览器标签页图标

#### 功能截图（推荐）

4. **main-interface.png** - 主界面
5. **region-capture.png** - 区域截图
6. **window-detection.png** - 窗口检测
7. **annotation-tools.png** - 标注工具
8. **color-picker.png** - 取色器
9. **pin-window.png** - 贴图功能
10. **settings.png** - 设置界面
11. **recording.png** - 录屏功能

### 截图规范

- **格式**：PNG（推荐）或 JPG
- **分辨率**：1920x1080 或更高
- **背景**：干净整洁，突出功能
- **内容**：清晰展示功能特点
- **文字**：确保文字清晰可读

### 如何准备截图

1. **运行 SnipX**
2. **使用 SnipX 自身截图**（展示功能）
3. **保存到指定目录**：`docs/public/screenshots/`
4. **命名规范**：使用上述文件名

---

## 🚀 本地测试

### 1. 安装依赖

```cmd
cd docs
npm install
```

### 2. 启动开发服务器

```cmd
npm run docs:dev
```

访问：http://localhost:5173

### 3. 检查网站

- ✅ 首页正常显示
- ✅ 导航栏链接正常
- ✅ 侧边栏链接正常
- ✅ 图片正常加载
- ✅ 搜索功能正常
- ✅ 多语言切换正常

### 4. 构建测试

```cmd
npm run docs:build
```

检查构建输出：`docs/.vitepress/dist/`

### 5. 预览构建结果

```cmd
npm run docs:preview
```

访问：http://localhost:4173

---

## 📦 部署到 GitHub Pages

### 方式 1：自动部署（推荐）

#### 步骤 1：推送代码到 GitHub

```cmd
cd c:\Users\yyyzz\Desktop\SnipX

# 如果还没有初始化 Git
git init
git add .
git commit -m "Add documentation website"

# 添加远程仓库
git remote add origin https://github.com/vogadero/SnipX.git

# 推送到 GitHub
git branch -M main
git push -u origin main
```

#### 步骤 2：启用 GitHub Pages

1. 访问：https://github.com/vogadero/SnipX/settings/pages
2. **Source** 选择：`GitHub Actions`
3. 保存设置

#### 步骤 3：触发部署

推送代码后，GitHub Actions 会自动构建和部署。

查看部署状态：https://github.com/vogadero/SnipX/actions

#### 步骤 4：访问网站

部署成功后，访问：https://vogadero.github.io/SnipX/

---

### 方式 2：手动部署

#### 步骤 1：构建网站

```cmd
cd docs
npm run docs:build
```

#### 步骤 2：部署到 gh-pages 分支

```cmd
cd .vitepress/dist

git init
git add -A
git commit -m "Deploy documentation"

git push -f https://github.com/vogadero/SnipX.git main:gh-pages
```

#### 步骤 3：配置 GitHub Pages

1. 访问：https://github.com/vogadero/SnipX/settings/pages
2. **Source** 选择：`Deploy from a branch`
3. **Branch** 选择：`gh-pages` / `root`
4. 保存设置

#### 步骤 4：访问网站

部署成功后，访问：https://vogadero.github.io/SnipX/

---

## 🔧 配置说明

### VitePress 配置

配置文件：`docs/.vitepress/config.mts`

#### 基本配置

```typescript
export default defineConfig({
  title: "SnipX",                    // 网站标题
  description: "...",                 // 网站描述
  base: '/SnipX/',                   // 基础路径（GitHub Pages）
  
  head: [
    ['link', { rel: 'icon', href: '/SnipX/favicon.ico' }]
  ]
})
```

#### 导航栏配置

```typescript
nav: [
  { text: '首页', link: '/' },
  { text: '功能特性', link: '/features' },
  { text: '下载', link: '/download' },
  { text: '文档', link: '/guide/getting-started' },
  { text: 'GitHub', link: 'https://github.com/vogadero/SnipX' }
]
```

#### 侧边栏配置

```typescript
sidebar: {
  '/guide/': [
    {
      text: '指南',
      items: [
        { text: '快速开始', link: '/guide/getting-started' },
        // ...
      ]
    }
  ]
}
```

### GitHub Actions 配置

配置文件：`docs/.github/workflows/deploy.yml`

#### 触发条件

```yaml
on:
  push:
    branches: [main]    # 推送到 main 分支时触发
  workflow_dispatch:    # 手动触发
```

#### 构建步骤

```yaml
- name: Install dependencies
  run: cd docs && npm ci

- name: Build with VitePress
  run: cd docs && npm run docs:build
```

---

## 📝 更新网站

### 更新内容

1. **编辑 Markdown 文件**
   ```cmd
   # 编辑文档
   notepad docs\index.md
   ```

2. **本地预览**
   ```cmd
   cd docs
   npm run docs:dev
   ```

3. **提交更改**
   ```cmd
   git add .
   git commit -m "Update documentation"
   git push
   ```

4. **自动部署**
   - GitHub Actions 会自动构建和部署
   - 等待几分钟后访问网站查看更新

### 添加新页面

1. **创建 Markdown 文件**
   ```cmd
   # 例如：添加 FAQ 页面
   notepad docs\faq.md
   ```

2. **更新配置**
   ```typescript
   // docs/.vitepress/config.mts
   nav: [
     // ...
     { text: 'FAQ', link: '/faq' }
   ]
   ```

3. **提交并推送**
   ```cmd
   git add .
   git commit -m "Add FAQ page"
   git push
   ```

### 更新截图

1. **替换截图文件**
   ```cmd
   # 将新截图放到 docs/public/screenshots/
   ```

2. **提交并推送**
   ```cmd
   git add docs/public/screenshots/
   git commit -m "Update screenshots"
   git push
   ```

---

## 🎨 自定义样式

### 添加自定义 CSS

1. **创建样式文件**
   ```cmd
   mkdir docs\.vitepress\theme
   notepad docs\.vitepress\theme\custom.css
   ```

2. **添加样式**
   ```css
   /* 自定义样式 */
   :root {
     --vp-c-brand: #3eaf7c;
   }
   ```

3. **导入样式**
   ```typescript
   // docs/.vitepress/theme/index.ts
   import DefaultTheme from 'vitepress/theme'
   import './custom.css'
   
   export default DefaultTheme
   ```

---

## 🌍 添加新语言

### 步骤 1：创建语言目录

```cmd
mkdir docs\ja
```

### 步骤 2：复制文档

```cmd
# 复制中文文档到日语目录
xcopy docs\*.md docs\ja\ /E
```

### 步骤 3：翻译内容

编辑 `docs/ja/` 目录下的文件，翻译成日语。

### 步骤 4：更新配置

```typescript
// docs/.vitepress/config.mts
locales: {
  // ...
  ja: {
    label: '日本語',
    lang: 'ja-JP',
    link: '/ja/',
    themeConfig: {
      nav: [
        { text: 'ホーム', link: '/ja/' },
        // ...
      ]
    }
  }
}
```

---

## 🔍 SEO 优化

### 添加 Meta 标签

```typescript
// docs/.vitepress/config.mts
head: [
  ['meta', { name: 'keywords', content: 'screenshot, windows, snipping tool' }],
  ['meta', { name: 'author', content: 'SnipX Team' }],
  ['meta', { property: 'og:title', content: 'SnipX' }],
  ['meta', { property: 'og:description', content: '...' }],
  ['meta', { property: 'og:image', content: '/hero-image.png' }]
]
```

### 添加 sitemap

VitePress 会自动生成 sitemap.xml。

### 添加 robots.txt

```cmd
# docs/public/robots.txt
User-agent: *
Allow: /
Sitemap: https://vogadero.github.io/SnipX/sitemap.xml
```

---

## 📊 分析和监控

### Google Analytics

```typescript
// docs/.vitepress/config.mts
head: [
  ['script', { 
    async: '', 
    src: 'https://www.googletagmanager.com/gtag/js?id=G-XXXXXXXXXX' 
  }],
  ['script', {}, `
    window.dataLayer = window.dataLayer || [];
    function gtag(){dataLayer.push(arguments);}
    gtag('js', new Date());
    gtag('config', 'G-XXXXXXXXXX');
  `]
]
```

---

## 🆘 常见问题

### Q1: 部署后 404 错误

**A**: 检查 `base` 配置是否正确：
```typescript
base: '/SnipX/',  // 必须与仓库名一致
```

### Q2: 图片不显示

**A**: 检查图片路径：
- 使用绝对路径：`/screenshots/image.png`
- 或相对路径：`./screenshots/image.png`

### Q3: 样式不生效

**A**: 清除缓存并重新构建：
```cmd
rm -rf docs/.vitepress/cache
npm run docs:build
```

### Q4: GitHub Actions 失败

**A**: 检查：
1. `package.json` 是否在 `docs/` 目录
2. Node.js 版本是否兼容
3. 查看 Actions 日志了解详细错误

### Q5: 本地开发正常，部署后异常

**A**: 检查：
1. `base` 配置是否正确
2. 所有链接是否使用正确的路径
3. 图片路径是否正确

---

## 📞 获取帮助

如果遇到问题：

- 📖 [VitePress 文档](https://vitepress.dev/)
- 📖 [GitHub Pages 文档](https://docs.github.com/pages)
- 🐛 [报告问题](https://github.com/vogadero/SnipX/issues)
- 💬 [讨论交流](https://github.com/vogadero/SnipX/discussions)

---

## ✅ 部署检查清单

### 部署前

- [ ] 所有截图已准备
- [ ] 本地测试通过
- [ ] 构建测试通过
- [ ] 预览测试通过
- [ ] 配置检查完成

### 部署中

- [ ] 代码已推送到 GitHub
- [ ] GitHub Pages 已启用
- [ ] GitHub Actions 运行成功
- [ ] 部署完成

### 部署后

- [ ] 网站可以访问
- [ ] 所有页面正常
- [ ] 所有链接正常
- [ ] 所有图片正常
- [ ] 搜索功能正常
- [ ] 多语言切换正常

---

## 🎉 完成

恭喜！你已经成功部署了 SnipX 文档网站！

**网站地址**：https://vogadero.github.io/SnipX/

现在可以：
- 📝 编写和更新文档
- 📸 添加更多截图
- 🌍 添加更多语言
- 🎨 自定义样式
- 📊 添加分析工具

---

**SnipX - 让截图回归简单与轻量** 🚀

**最后更新**：2024-01-15
