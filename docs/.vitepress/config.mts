import { defineConfig } from 'vitepress'

export default defineConfig({
  title: "SnipX",
  description: "极致轻量的 Windows 截图工具 - Lightweight Screenshot Tool for Windows",
  base: '/SnipX/',
  
  head: [
    ['link', { rel: 'icon', href: '/SnipX/favicon.ico' }]
  ],

  locales: {
    root: {
      label: '简体中文',
      lang: 'zh-CN',
      themeConfig: {
        nav: [
          { text: '首页', link: '/' },
          { text: '功能特性', link: '/features' },
          { text: '下载', link: '/download' },
          { text: '文档', link: '/guide/getting-started' },
          { text: 'GitHub', link: 'https://github.com/vogadero/SnipX' }
        ],

        sidebar: {
          '/guide/': [
            {
              text: '指南',
              items: [
                { text: '快速开始', link: '/guide/getting-started' },
                { text: '基础功能', link: '/guide/basic-features' },
                { text: '标注工具', link: '/guide/annotation-tools' },
                { text: '高级功能', link: '/guide/advanced-features' },
                { text: '快捷键', link: '/guide/shortcuts' },
                { text: '设置选项', link: '/guide/settings' }
              ]
            },
            {
              text: '开发',
              items: [
                { text: '编译指南', link: '/guide/compile' },
                { text: '贡献指南', link: '/guide/contributing' },
                { text: 'API 文档', link: '/guide/api' }
              ]
            }
          ]
        },

        footer: {
          message: 'Released under the MIT License.',
          copyright: 'Copyright © 2024 SnipX Team'
        },

        socialLinks: [
          { icon: 'github', link: 'https://github.com/vogadero/SnipX' }
        ],

        search: {
          provider: 'local'
        }
      }
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      themeConfig: {
        nav: [
          { text: 'Home', link: '/en/' },
          { text: 'Features', link: '/en/features' },
          { text: 'Download', link: '/en/download' },
          { text: 'Docs', link: '/en/guide/getting-started' },
          { text: 'GitHub', link: 'https://github.com/vogadero/SnipX' }
        ],

        sidebar: {
          '/en/guide/': [
            {
              text: 'Guide',
              items: [
                { text: 'Getting Started', link: '/en/guide/getting-started' },
                { text: 'Basic Features', link: '/en/guide/basic-features' },
                { text: 'Annotation Tools', link: '/en/guide/annotation-tools' },
                { text: 'Advanced Features', link: '/en/guide/advanced-features' },
                { text: 'Shortcuts', link: '/en/guide/shortcuts' },
                { text: 'Settings', link: '/en/guide/settings' }
              ]
            },
            {
              text: 'Development',
              items: [
                { text: 'Compile Guide', link: '/en/guide/compile' },
                { text: 'Contributing', link: '/en/guide/contributing' },
                { text: 'API Documentation', link: '/en/guide/api' }
              ]
            }
          ]
        },

        footer: {
          message: 'Released under the MIT License.',
          copyright: 'Copyright © 2024 SnipX Team'
        }
      }
    }
  },

  themeConfig: {
    logo: '/logo.png'
  }
})
