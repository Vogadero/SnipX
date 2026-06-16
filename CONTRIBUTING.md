# 贡献指南

感谢你对 SnipX 项目的关注！我们欢迎各种形式的贡献。

## 如何贡献

### 报告 Bug

如果你发现了 Bug，请在 [GitHub Issues](https://github.com/vogadero/SnipX/issues) 创建一个 issue，并包含以下信息：

1. **Bug 描述**：清晰简洁地描述问题
2. **复现步骤**：详细的复现步骤
3. **预期行为**：你期望发生什么
4. **实际行为**：实际发生了什么
5. **环境信息**：
   - 操作系统版本（如 Windows 10 21H2）
   - SnipX 版本
   - 屏幕分辨率和 DPI 设置
6. **截图或日志**：如果可能，提供截图或日志文件

### 建议新功能

如果你有新功能的想法，请创建一个 issue 并包含：

1. **功能描述**：详细描述功能
2. **使用场景**：说明为什么需要这个功能
3. **预期效果**：功能应该如何工作
4. **替代方案**：是否考虑过其他方案

### 提交代码

#### 准备工作

1. **Fork 项目**
   ```bash
   # 在 GitHub 上点击 Fork 按钮
   ```

2. **克隆仓库**
   ```bash
   git clone https://github.com/your-username/SnipX.git
   cd SnipX
   ```

3. **创建分支**
   ```bash
   git checkout -b feature/your-feature-name
   # 或
   git checkout -b fix/bug-description
   ```

#### 开发规范

1. **代码风格**
   - 遵循项目的编码规范（见 `DEVELOPER_GUIDE.md`）
   - 使用 4 空格缩进
   - 大括号另起一行
   - 添加必要的注释

2. **提交信息**
   - 使用 Conventional Commits 格式
   - 格式：`<type>(<scope>): <subject>`
   - 类型：
     - `feat`: 新功能
     - `fix`: Bug 修复
     - `docs`: 文档更新
     - `style`: 代码格式（不影响功能）
     - `refactor`: 重构
     - `perf`: 性能优化
     - `test`: 测试相关
     - `chore`: 构建/工具相关

   示例：
   ```
   feat(editor): add blur tool
   fix(capture): fix window detection on Win7
   docs(readme): update installation instructions
   ```

3. **代码质量**
   - 确保代码可以编译通过
   - 添加必要的错误处理
   - 添加日志记录
   - 测试你的修改

#### 提交流程

1. **提交更改**
   ```bash
   git add .
   git commit -m "feat(editor): add blur tool"
   ```

2. **推送到 GitHub**
   ```bash
   git push origin feature/your-feature-name
   ```

3. **创建 Pull Request**
   - 访问你的 Fork 仓库
   - 点击 "New Pull Request"
   - 填写 PR 描述：
     - 修改内容
     - 相关 Issue
     - 测试情况
     - 截图（如果适用）

4. **代码审查**
   - 维护者会审查你的代码
   - 根据反馈进行修改
   - 保持 PR 更新

#### Pull Request 检查清单

提交 PR 前，请确认：

- [ ] 代码遵循项目规范
- [ ] 添加了必要的注释
- [ ] 代码可以编译通过
- [ ] 功能已测试
- [ ] 更新了相关文档
- [ ] 提交信息符合规范
- [ ] 没有引入新的警告
- [ ] 没有破坏现有功能

### 改进文档

文档改进同样重要！你可以：

- 修正拼写或语法错误
- 改进文档结构
- 添加示例代码
- 翻译文档
- 补充缺失的内容

文档修改流程与代码提交相同。

### 测试

帮助测试也是重要的贡献：

1. **功能测试**：测试新功能和现有功能
2. **兼容性测试**：在不同系统上测试
3. **性能测试**：测试性能和资源占用
4. **Bug 验证**：验证 Bug 修复是否有效

测试结果可以在相关 Issue 中反馈。

## 开发环境

### 必需工具

- Visual Studio 2019/2022 或 MinGW-w64
- Git
- CMake（可选）

### 编译项目

```bash
# 使用 build.bat
build.bat

# 或使用 CMake
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

详细说明见 `COMPILE.md`。

## 代码审查标准

维护者会根据以下标准审查代码：

### 功能性
- [ ] 功能正确实现
- [ ] 没有引入新 Bug
- [ ] 边界情况处理正确

### 代码质量
- [ ] 代码清晰易读
- [ ] 遵循项目规范
- [ ] 适当的错误处理
- [ ] 合理的性能

### 文档
- [ ] 公共 API 有文档
- [ ] 复杂逻辑有注释
- [ ] 更新了相关文档

### 测试
- [ ] 功能已测试
- [ ] 没有破坏现有功能

## 社区规范

### 行为准则

我们致力于提供友好、安全和包容的环境。参与者应该：

- **尊重他人**：尊重不同的观点和经验
- **建设性反馈**：提供有帮助的、建设性的反馈
- **保持专业**：使用专业和友好的语言
- **接受批评**：优雅地接受建设性批评

不可接受的行为包括：

- 使用性别化语言或图像
- 人身攻击或侮辱
- 骚扰行为
- 发布他人的私人信息
- 其他不专业或不受欢迎的行为

### 沟通渠道

- **GitHub Issues**：Bug 报告和功能请求
- **GitHub Discussions**：一般讨论和问题
- **Pull Requests**：代码贡献

## 许可证

通过贡献代码，你同意你的贡献将在 MIT 许可证下发布。

## 问题？

如果你有任何问题，可以：

1. 查看 [FAQ](https://github.com/vogadero/SnipX/wiki/FAQ)
2. 在 [Discussions](https://github.com/vogadero/SnipX/discussions) 提问

## 致谢

感谢所有贡献者！你们的贡献让 SnipX 变得更好。

### 贡献者列表

查看 [Contributors](https://github.com/vogadero/SnipX/graphs/contributors)

---

**再次感谢你的贡献！** 🎉
