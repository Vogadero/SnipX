# SnipX - Ultra-Lightweight Screenshot Tool

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/language-C%2B%2B17-orange.svg)](https://isocpp.org/)
[![Size](https://img.shields.io/badge/size-%3C1MB-green.svg)](#)
[![Status](https://img.shields.io/badge/status-Stable-brightgreen.svg)](#)

> Ultra-lightweight Windows screenshot, annotation, pinning, color picker, and recording tool built with pure Win32 C++17 and GDI+.

[English](#) | [简体中文](README.md)

## ✨ Features

- 🪶 **Ultra-Lightweight**: Target size < 1MB, startup < 500ms
- 🚀 **Pure Win32**: Built with C++ + Win32 API + GDI+, no third-party dependencies
- 📦 **Portable**: Single exe file, no installation required
- ⚡ **High Performance**: Memory < 10MB, CPU < 1%
- 🎨 **Full-Featured**: Screenshot, annotation, editing, saving, pinning, recording
- 🌍 **Multi-Language**: Support for Chinese, English, and more
- 🔧 **Extensible**: Modular design, easy to extend and customize

## 📸 Screenshots

### Core Features

- ✅ **Region Capture** - Drag to select any area
- ✅ **Full Screen** - Capture entire screen with one click
- ✅ **Window Detection** - Auto-detect window boundaries
- ✅ **Scrolling Capture** - Capture long webpages and documents
- ✅ **Multi-Monitor** - Perfect support for multiple displays

### Annotation Tools (9 types)

- ✅ **Rectangle** - Draw rectangle borders
- ✅ **Ellipse** - Draw ellipse borders
- ✅ **Arrow** - Add arrow indicators
- ✅ **Line** - Draw straight lines
- ✅ **Pencil** - Free drawing
- ✅ **Text** - Add text annotations
- ✅ **Mosaic** - Pixelate areas
- ✅ **Blur** - Gaussian blur
- ✅ **Number** - Add numbered labels

### Advanced Features

- ✅ **Color Picker** - RGB/HSL/HEX color picking
- ✅ **Pin to Desktop** - Pin screenshots to desktop with zoom and transparency
- ✅ **Screen Recording** - Record screen and export to GIF
- ✅ **OCR** - Text recognition (extension)
- ✅ **Upload** - Image hosting upload (extension)

## 🚀 Quick Start

### Download

- [Latest Release](https://github.com/Vogadero/SnipX/releases/latest)
- [All Releases](https://github.com/Vogadero/SnipX/releases)

### Usage

1. Download `SnipX.exe`
2. Double-click to run
3. Press `Alt + X` to start region capture, or `Alt + F` for full-screen capture

For detailed instructions, see [User Manual](USER_MANUAL.md)

## 📋 System Requirements

- **OS**: Windows 7 or higher
- **Architecture**: x64
- **Dependencies**: None (uses built-in GDI+)

## 🛠️ Tech Stack

- **Language**: C++ 17
- **UI Framework**: Win32 API
- **Graphics**: GDI+ (built-in)
- **Compiler**: MSVC or MinGW-w64
- **Build Tool**: CMake (optional)

## 📦 Building from Source

### Prerequisites

- Visual Studio 2019/2022 or MinGW-w64
- Git
- CMake (optional)

### Build Steps

#### Using build.bat

```cmd
git clone https://github.com/Vogadero/SnipX.git
cd SnipX
build.bat
```

#### Using CMake

```cmd
git clone https://github.com/Vogadero/SnipX.git
cd SnipX
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

For detailed build instructions, see [COMPILE.md](COMPILE.md)

## 📖 Documentation

### User Documentation
- [README](README.md) - Project overview
- [Quick Start](QUICKSTART.md) - 5-minute guide
- [User Manual](USER_MANUAL.md) - Detailed usage guide
- [Changelog](CHANGELOG.md) - Version history

### Developer Documentation
- [Developer Guide](DEVELOPER_GUIDE.md) - Complete development guide
- [Compile Guide](COMPILE.md) - Build instructions
- [Architecture](ARCHITECTURE.md) - Architecture design
- [API Documentation](API_DOCUMENTATION.md) - API reference
- [Contributing](CONTRIBUTING.md) - Contribution guidelines

### Technical Documentation
- [Performance Optimization](PERFORMANCE_OPTIMIZATION.md) - Optimization guide
- [Task and Test Checklist](TODO.md) - Remaining validation checklist
- [Code Comments Guide](CODE_COMMENTS_GUIDE.md) - Comment standards

## 🎯 Roadmap

### v1.1.3 - Portable Startup Fix (Current)
- ✅ Removed the Windows manifest compatibility section that could make activation context creation fail and cause the "side-by-side configuration is incorrect" startup error
- ✅ Fixed MSVC build missing `/utf-8` compile option causing Chinese text garbled in the UI

### v1.1.2 - Portable Startup Fix
- ✅ Removed the Windows manifest compatibility section that could make activation context creation fail and cause the "side-by-side configuration is incorrect" startup error

### v1.1.1 - Portable Startup Fix
- ✅ Fixed portable Windows manifest architecture declaration to avoid the "side-by-side configuration is incorrect" startup error

### v1.1.0 - Enhancement
- ✅ Number keys to switch annotation tools
- ✅ Pin window shortcuts: `Ctrl+C`, `Ctrl+S`, `Esc`
- ✅ Color picker `C` key to copy the current display mode
- ✅ `R` / `H` to enter the color picker from capture mode
- ✅ Logger format string security fix
- ✅ Scroll capture null pointer and hotkey registration fixes

### v1.0.0 - Initial Release
- ✅ All core features
- ✅ Complete documentation
- ✅ Release system

### v2.0.0 - Advanced
- Plugin system
- Cloud sync
- Cross-platform support

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

### How to Contribute

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## 🐛 Bug Reports

Found a bug? Please [create an issue](https://github.com/Vogadero/SnipX/issues/new) with:

- Bug description
- Steps to reproduce
- Expected behavior
- Actual behavior
- Environment info (OS version, SnipX version, etc.)

## 💡 Feature Requests

Have an idea? Please [create an issue](https://github.com/Vogadero/SnipX/issues/new) with:

- Feature description
- Use case
- Expected behavior
- Alternative solutions

## 🔒 Security

For security issues, please see [SECURITY.md](SECURITY.md) for reporting guidelines.

## 📄 License

SnipX is licensed under the [MIT License](LICENSE).

## 🙏 Acknowledgments

Thanks to all contributors and users!

Special thanks to:
- All testers
- All feedback providers
- All contributors

## 📞 Contact

- **GitHub**: https://github.com/Vogadero/SnipX
- **Issues**: https://github.com/Vogadero/SnipX/issues
- **Discussions**: https://github.com/Vogadero/SnipX/discussions

## ⭐ Star History

If you like SnipX, please give it a star! ⭐

## 📊 Project Stats

- **Language**: C++ 17
- **Lines of Code**: ~6,375
- **Documentation**: ~46,300 words
- **Files**: 60+
- **Completion**: 98%

## 🎓 Learn More

- [Architecture Design](ARCHITECTURE.md)
- [Performance Optimization](PERFORMANCE_OPTIMIZATION.md)
- [API Documentation](API_DOCUMENTATION.md)
- [Developer Guide](DEVELOPER_GUIDE.md)

---

**SnipX - Making Screenshots Simple and Lightweight** 🚀

**Made with ❤️ by SnipX Team**

**Last Updated**: 2026-06-05
