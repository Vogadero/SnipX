#!/usr/bin/env python3
"""
SnipX 图标生成脚本
使用 Pillow 库生成简单的截图工具图标
"""

try:
    from PIL import Image, ImageDraw
except ImportError:
    print("错误：未安装 Pillow 库")
    print("请运行：pip install Pillow")
    exit(1)

def create_icon():
    """创建 SnipX 图标"""
    
    # 创建多个尺寸
    sizes = [16, 32, 48, 256]
    images = []
    
    for size in sizes:
        # 创建透明背景图像
        img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # 计算比例
        scale = size / 32.0
        
        # 主色：Windows 蓝
        color = (0, 120, 215, 255)
        
        # 绘制外框（选区框）
        padding = int(4 * scale)
        border_width = max(1, int(2 * scale))
        
        # 外框
        draw.rectangle(
            [padding, padding, size - padding, size - padding],
            outline=color,
            width=border_width
        )
        
        # 绘制角点（表示可调整大小）
        corner_size = max(2, int(4 * scale))
        corners = [
            (padding, padding),  # 左上
            (size - padding - corner_size, padding),  # 右上
            (padding, size - padding - corner_size),  # 左下
            (size - padding - corner_size, size - padding - corner_size)  # 右下
        ]
        
        for x, y in corners:
            draw.rectangle(
                [x, y, x + corner_size, y + corner_size],
                fill=color
            )
        
        # 绘制中心十字（表示精确定位）
        if size >= 32:
            center = size // 2
            cross_size = int(6 * scale)
            cross_width = max(1, int(2 * scale))
            
            # 横线
            draw.line(
                [center - cross_size, center, center + cross_size, center],
                fill=color,
                width=cross_width
            )
            # 竖线
            draw.line(
                [center, center - cross_size, center, center + cross_size],
                fill=color,
                width=cross_width
            )
        
        images.append(img)
    
    # 保存为 ICO 文件（包含多个尺寸）
    images[0].save(
        'res/icon.ico',
        format='ICO',
        sizes=[(s, s) for s in sizes],
        append_images=images[1:]
    )
    
    print("✓ 图标已生成：res/icon.ico")
    print(f"  包含尺寸：{', '.join(f'{s}x{s}' for s in sizes)}")
    
    # 同时保存为 PNG 预览
    images[2].save('res/icon_preview.png')
    print("✓ 预览图已生成：res/icon_preview.png")

if __name__ == '__main__':
    import os
    
    # 确保 res 目录存在
    if not os.path.exists('res'):
        os.makedirs('res')
        print("✓ 创建 res 目录")
    
    create_icon()
    print("\n图标生成完成！现在可以编译项目了。")
