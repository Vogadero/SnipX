import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'SnipX',
  description: '极致轻量的 Windows 截图工具 - Lightweight Screenshot Tool for Windows',
  base: '/SnipX/',

  head: [
    ['link', { rel: 'icon', href: '/SnipX/favicon.ico' }]
  ],

  themeConfig: {
    logo: '/logo.png',

    nav: [
      { text: '首页', link: '/' },
      { text: '功能特性', link: '/features' },
      { text: '下载', link: '/download' },
      { text: '快速开始', link: '/guide/getting-started' },
      { text: '快捷键', link: '/guide/shortcuts' },
      { text: 'GitHub', link: 'https://github.com/vogadero/SnipX' }
    ],

    sidebar: {
      '/guide/': [
        {
          text: '指南',
          items: [
            { text: '快速开始', link: '/guide/getting-started' },
            { text: '快捷键', link: '/guide/shortcuts' }
          ]
        }
      ]
    },

    footer: {
      message: 'Released under the MIT License.',
      copyright: 'Copyright © 2026 SnipX Team'
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/vogadero/SnipX' }
    ],

    search: {
      provider: 'local'
    }
  }
})
