// 跨平台适配层

#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(_WIN32)
#define SNIPX_PLATFORM_WINDOWS 1
#define SNIPX_PLATFORM_NAME L"Windows"
#elif defined(__APPLE__)
#define SNIPX_PLATFORM_MACOS 1
#define SNIPX_PLATFORM_NAME L"macOS"
#elif defined(__linux__)
#define SNIPX_PLATFORM_LINUX 1
#define SNIPX_PLATFORM_NAME L"Linux"
#else
#define SNIPX_PLATFORM_UNKNOWN 1
#define SNIPX_PLATFORM_NAME L"Unknown"
#endif

/**
 * 返回当前构建目标平台名称。
 *
 * @return 平台名称宽字符串。
 */
const wchar_t* GetPlatformName();

/**
 * 返回当前构建是否启用完整 Win32 功能。
 *
 * @return Windows 构建返回 true，其他平台返回 false。
 */
bool IsFullFeaturePlatform();

#endif // PLATFORM_H
