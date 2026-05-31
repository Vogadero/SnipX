# 安全策略 / Security Policy

## 支持的版本 / Supported Versions

我们目前支持以下版本的安全更新：

| 版本 / Version | 支持状态 / Supported |
| -------------- | ------------------- |
| 1.0.x          | :white_check_mark:  |
| < 1.0          | :x:                 |

## 报告漏洞 / Reporting a Vulnerability

### 中文

如果你发现了 SnipX 的安全漏洞，请**不要**公开披露。请通过以下方式私下报告：

1. **邮件报告**（推荐）
   - 发送邮件到：security@snipx.example.com
   - 主题：[Security] 简要描述漏洞
   - 内容包括：
     - 漏洞详细描述
     - 复现步骤
     - 影响范围
     - 建议的修复方案（如果有）

2. **GitHub Security Advisory**
   - 访问：https://github.com/yourusername/SnipX/security/advisories
   - 点击 "Report a vulnerability"
   - 填写漏洞详情

### 响应时间

- **确认收到**：24 小时内
- **初步评估**：3 个工作日内
- **修复计划**：7 个工作日内
- **发布补丁**：根据严重程度，14-30 天内

### 漏洞等级

我们使用 CVSS 3.1 标准评估漏洞严重程度：

- **严重 (Critical)**: CVSS 9.0-10.0
  - 立即修复，7 天内发布补丁
  
- **高危 (High)**: CVSS 7.0-8.9
  - 优先修复，14 天内发布补丁
  
- **中危 (Medium)**: CVSS 4.0-6.9
  - 计划修复，30 天内发布补丁
  
- **低危 (Low)**: CVSS 0.1-3.9
  - 常规修复，下一个版本中修复

### 奖励计划

目前我们没有正式的漏洞奖励计划，但我们会：

- 在致谢名单中感谢报告者
- 在发布说明中提及贡献
- 提供 SnipX 周边（如果有）

### 安全最佳实践

使用 SnipX 时，请遵循以下安全建议：

1. **下载来源**
   - 仅从官方 GitHub Releases 下载
   - 验证文件哈希值
   - 不要从第三方网站下载

2. **权限管理**
   - SnipX 不需要管理员权限
   - 不要以管理员身份运行（除非必要）

3. **配置文件**
   - 配置文件位于 `%APPDATA%\SnipX\`
   - 不要与他人共享配置文件
   - 定期备份配置

4. **更新**
   - 及时更新到最新版本
   - 关注安全公告
   - 启用自动更新检查

### 已知安全限制

1. **截图内容**
   - SnipX 可以截取屏幕上的任何内容
   - 请注意不要截取敏感信息
   - 截图文件未加密

2. **日志文件**
   - 日志文件可能包含文件路径等信息
   - 日志文件位于 `%APPDATA%\SnipX\snipx.log`
   - 定期清理日志文件

3. **网络功能**
   - 更新检查会连接到 GitHub API
   - 图片上传功能会连接到图床服务
   - 可以在设置中禁用网络功能

### 安全审计

我们欢迎安全研究人员审计 SnipX 的代码：

- 源代码：https://github.com/yourusername/SnipX
- 许可证：MIT License
- 审计范围：所有功能和代码

---

## English

If you discover a security vulnerability in SnipX, please **do not** disclose it publicly. Report it privately through:

1. **Email** (Recommended)
   - Send to: security@snipx.example.com
   - Subject: [Security] Brief description
   - Include:
     - Detailed description
     - Steps to reproduce
     - Impact assessment
     - Suggested fix (if any)

2. **GitHub Security Advisory**
   - Visit: https://github.com/yourusername/SnipX/security/advisories
   - Click "Report a vulnerability"
   - Fill in the details

### Response Time

- **Acknowledgment**: Within 24 hours
- **Initial Assessment**: Within 3 business days
- **Fix Plan**: Within 7 business days
- **Patch Release**: 14-30 days depending on severity

### Severity Levels

We use CVSS 3.1 to assess vulnerability severity:

- **Critical**: CVSS 9.0-10.0 → Patch within 7 days
- **High**: CVSS 7.0-8.9 → Patch within 14 days
- **Medium**: CVSS 4.0-6.9 → Patch within 30 days
- **Low**: CVSS 0.1-3.9 → Fix in next release

### Reward Program

Currently, we don't have a formal bug bounty program, but we will:

- Credit you in our acknowledgments
- Mention your contribution in release notes
- Provide SnipX merchandise (if available)

### Security Best Practices

When using SnipX:

1. **Download Source**
   - Only download from official GitHub Releases
   - Verify file hashes
   - Don't download from third-party sites

2. **Permissions**
   - SnipX doesn't require admin rights
   - Don't run as administrator unless necessary

3. **Configuration**
   - Config files are in `%APPDATA%\SnipX\`
   - Don't share config files
   - Backup regularly

4. **Updates**
   - Update to the latest version
   - Follow security announcements
   - Enable auto-update check

### Known Security Limitations

1. **Screenshot Content**
   - SnipX can capture any screen content
   - Be careful with sensitive information
   - Screenshots are not encrypted

2. **Log Files**
   - Logs may contain file paths
   - Located at `%APPDATA%\SnipX\snipx.log`
   - Clean up regularly

3. **Network Features**
   - Update check connects to GitHub API
   - Image upload connects to image hosting
   - Can be disabled in settings

---

**Thank you for helping keep SnipX secure!**

**感谢你帮助保护 SnipX 的安全！**

**Last Updated**: 2024-01-15
