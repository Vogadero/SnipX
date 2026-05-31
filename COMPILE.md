# SnipX 编译指南

## 前置要求

### 方法 1：使用 Visual Studio（推荐）

1. 安装 Visual Studio 2019 或更高版本
2. 安装时选择"使用 C++ 的桌面开发"工作负载
3. 确保安装了 Windows SDK

### 方法 2：使用 MinGW-w64

1. 下载 MinGW-w64：https://www.mingw-w64.org/downloads/
2. 推荐使用 MSYS2 安装：
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-gdb
   ```
3. 将 MinGW 的 bin 目录添加到系统 PATH

## 编译步骤

### 快速编译（使用 build.bat）

1. 打开命令提示符（CMD）
2. 进入项目目录：
   ```cmd
   cd C:\Users\yyyzz\Desktop\SnipX
   ```
3. 运行编译脚本：
   ```cmd
   build.bat
   ```
4. 编译成功后，可执行文件位于 `bin\SnipX.exe`

### 使用 Visual Studio 命令行

1. 打开 "Developer Command Prompt for VS"
2. 进入项目目录
3. 运行：
   ```cmd
   build.bat
   ```

### 使用 CMake（跨平台）

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

或使用 MinGW：

```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## 编译优化

### 体积优化

当前编译脚本已包含以下优化：

**MSVC 优化选项：**
- `/O2` - 最大速度优化
- `/GL` - 全程序优化
- `/LTCG` - 链接时代码生成
- `/OPT:REF` - 移除未引用函数
- `/OPT:ICF` - 合并相同函数

**MinGW 优化选项：**
- `-O3` - 最高级别优化
- `-s` - 移除符号表
- `-static` - 静态链接

### 进一步优化（高级）

如需进一步减小体积，可以：

1. **使用 UPX 压缩**（可减小 50-70%）：
   ```cmd
   upx --best --lzma bin\SnipX.exe
   ```

2. **手动优化链接选项**：
   - 移除调试信息
   - 使用 `/MERGE` 合并代码段
   - 使用 `/ALIGN:512` 减小对齐

3. **代码级优化**：
   - 减少 STL 使用
   - 使用 C 风格字符串
   - 内联小函数

## 常见问题

### 1. 找不到 gdiplus.lib

**解决方案：**
- 确保安装了 Windows SDK
- 检查 Visual Studio 安装是否完整

### 2. 编译时提示缺少 icon.ico

**解决方案：**
- 在 `res/` 目录下放置 icon.ico 文件
- 或临时注释掉 `res/SnipX.rc` 中的图标引用

### 3. MinGW 编译失败

**解决方案：**
- 确保 MinGW 的 bin 目录在 PATH 中
- 使用 `where g++` 检查编译器是否可用
- 尝试使用 MSYS2 安装的 MinGW

### 4. 链接错误：找不到 dwmapi.lib

**解决方案：**
- 更新 Windows SDK
- 或在代码中移除 DWM 相关功能（窗口阴影检测）

## 编译结果

成功编译后，你应该看到：

```
编译成功！
 驱动器 C 中的卷是 Windows
 卷的序列号是 XXXX-XXXX

 C:\Users\yyyzz\Desktop\SnipX\bin 的目录

2026/04/30  21:42           XXX,XXX SnipX.exe
               1 个文件    XXX,XXX 字节
```

目标体积：< 1 MB（1,048,576 字节）

## 运行测试

编译完成后，可以直接运行：

```cmd
bin\SnipX.exe
```

测试功能：
1. 程序启动后应在系统托盘显示图标
2. 按 Alt+X 应触发截图
3. 双击托盘图标应触发截图
4. 右键托盘图标应显示菜单

## 调试

如需调试，使用 Debug 配置编译：

**MSVC：**
```cmd
cl /c /Od /Zi /MDd /W3 /nologo ...
link /DEBUG ...
```

**MinGW：**
```cmd
g++ -g -O0 ...
```

然后使用 Visual Studio 或 GDB 进行调试。
