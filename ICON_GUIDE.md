# 图标创建指南

## 快速解决方案

如果你想快速编译项目而不想处理图标问题，可以临时注释掉图标引用：

### 方法 1：注释掉图标（推荐用于快速测试）

编辑 `res/SnipX.rc` 文件，注释掉图标行：

```rc
// IDI_MAINICON            ICON                    "icon.ico"
```

### 方法 2：使用 Windows 系统图标

修改 `src/TrayIcon.cpp` 中的图标加载代码：

```cpp
// 原代码：
m_nid.hIcon = LoadIcon(pApp->GetInstance(), MAKEINTRESOURCE(IDI_MAINICON));

// 改为使用系统图标：
m_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
// 或者：
m_nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
```

## 创建自定义图标

### 在线工具（最简单）

1. **Favicon Generator**
   - 网址：https://www.favicon-generator.org/
   - 上传一张图片（PNG/JPG）
   - 下载生成的 .ico 文件
   - 重命名为 `icon.ico` 并放到 `res/` 目录

2. **ICO Convert**
   - 网址：https://icoconvert.com/
   - 支持多种格式转换
   - 可以选择多个尺寸

3. **RealFaviconGenerator**
   - 网址：https://realfavicongenerator.net/
   - 功能强大，支持预览

### 使用 GIMP（免费软件）

1. 下载安装 GIMP：https://www.gimp.org/
2. 创建新图像：32x32 像素
3. 设计你的图标（建议使用剪刀或截图符号）
4. 导出为 .ico 格式
5. 保存到 `res/icon.ico`

### 使用 Paint.NET（Windows）

1. 下载 Paint.NET：https://www.getpaint.net/
2. 安装 ICO 插件
3. 创建 32x32 图像
4. 保存为 .ico 格式

### 从现有软件提取图标

使用 ResourceHacker 工具：
1. 下载 ResourceHacker：http://www.angusj.com/resourcehacker/
2. 打开任意 .exe 文件
3. 导航到 Icon Group
4. 导出图标
5. 保存为 `icon.ico`

## 图标设计建议

### 尺寸要求
- 16x16（小图标）
- 32x32（标准）
- 48x48（大图标）
- 256x256（高清）

建议创建包含多个尺寸的 .ico 文件。

### 设计风格
- **简洁**：使用简单的几何形状
- **识别性**：一眼能看出是截图工具
- **对比度**：确保在深色和浅色背景下都清晰

### 推荐符号
- ✂️ 剪刀（表示"剪切"屏幕）
- 📷 相机
- 🖼️ 图片框
- ⬜ 选区框
- 🔲 虚线框

### 颜色建议
- 主色：蓝色（#0078D4，Windows 主题色）
- 辅色：白色或浅灰
- 避免过多颜色（2-3种即可）

## 简单的 ASCII 图标设计

如果你想用代码生成简单图标，可以参考这个 32x32 的设计：

```
████████████████████████████████
██                            ██
██  ████████████████████████  ██
██  ██                    ██  ██
██  ██  ████████████████  ██  ██
██  ██  ██            ██  ██  ██
██  ██  ██  ████████  ██  ██  ██
██  ██  ██  ██    ██  ██  ██  ██
██  ██  ██  ██    ██  ██  ██  ██
██  ██  ██  ████████  ██  ██  ██
██  ██  ██            ██  ██  ██
██  ██  ████████████████  ██  ██
██  ██                    ██  ██
██  ████████████████████████  ██
██                            ██
████████████████████████████████
```

这表示一个"选区框"的概念。

## 使用 Python 生成简单图标

如果你安装了 Python 和 Pillow 库：

```python
from PIL import Image, ImageDraw

# 创建 32x32 图像
img = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# 绘制外框（蓝色）
draw.rectangle([4, 4, 28, 28], outline=(0, 120, 215, 255), width=2)

# 绘制内框（虚线效果）
for i in range(8, 24, 4):
    draw.line([i, 8, i+2, 8], fill=(0, 120, 215, 255), width=1)
    draw.line([i, 24, i+2, 24], fill=(0, 120, 215, 255), width=1)
    draw.line([8, i, 8, i+2], fill=(0, 120, 215, 255), width=1)
    draw.line([24, i, 24, i+2], fill=(0, 120, 215, 255), width=1)

# 保存为 ICO
img.save('res/icon.ico', format='ICO', sizes=[(32, 32)])
print("图标已生成：res/icon.ico")
```

运行：
```bash
pip install Pillow
python generate_icon.py
```

## 验证图标

创建图标后，可以：
1. 在 Windows 资源管理器中预览
2. 使用 IrfanView 等工具查看
3. 编译项目，查看托盘图标效果

## 常见问题

### Q: 图标显示模糊？
A: 确保创建了多个尺寸（16x16, 32x32, 48x48），Windows 会自动选择合适的尺寸。

### Q: 图标背景不透明？
A: 使用支持透明度的格式（PNG），并在转换为 ICO 时保留 Alpha 通道。

### Q: 编译时找不到图标？
A: 检查文件路径是否正确：`res/icon.ico`，注意大小写。

### Q: 不想创建图标怎么办？
A: 使用方法 1 或方法 2（见本文开头），使用系统图标或注释掉图标引用。
