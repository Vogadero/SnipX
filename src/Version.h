// SnipX 版本信息

#ifndef VERSION_H
#define VERSION_H

/**
 * 主版本号。
 */
#define SNIPX_VERSION_MAJOR 1
/**
 * 次版本号。
 */
#define SNIPX_VERSION_MINOR 1
/**
 * 修订版本号。
 */
#define SNIPX_VERSION_PATCH 8
/**
 * 构建号。
 */
#define SNIPX_VERSION_BUILD 0

/**
 * 对外展示的版本字符串。
 */
#define SNIPX_VERSION_STRING L"1.1.8"
/**
 * 含构建号的完整版本字符串。
 */
#define SNIPX_VERSION_STRING_FULL L"1.1.8.0"

// 产品信息
#define SNIPX_PRODUCT_NAME L"SnipX"
#define SNIPX_PRODUCT_DESCRIPTION L"极致轻量的 Windows 截图工具"
#define SNIPX_COMPANY_NAME L"SnipX Team"
#define SNIPX_COPYRIGHT L"Copyright © 2024 SnipX Team. All rights reserved."

// 构建信息
#define SNIPX_BUILD_DATE __DATE__
#define SNIPX_BUILD_TIME __TIME__

/**
 * 获取版本字符串。
 *
 * @return 如 L"1.1.8"。
 */
inline const wchar_t* GetVersionString()
{
    return SNIPX_VERSION_STRING;
}

/**
 * 获取完整版本字符串。
 *
 * @return 如 L"1.1.8.0"。
 */
inline const wchar_t* GetFullVersionString()
{
    return SNIPX_VERSION_STRING_FULL;
}

/**
 * 获取产品名称。
 */
inline const wchar_t* GetProductName()
{
    return SNIPX_PRODUCT_NAME;
}

/**
 * 获取产品描述。
 */
inline const wchar_t* GetProductDescription()
{
    return SNIPX_PRODUCT_DESCRIPTION;
}

/**
 * 获取版权信息。
 */
inline const wchar_t* GetCopyright()
{
    return SNIPX_COPYRIGHT;
}

/**
 * 获取构建日期时间字符串。
 *
 * @return 静态缓冲区中的 "date time" 文本。
 */
inline const char* GetBuildDateTime()
{
    static char buffer[64];
    sprintf_s(buffer, "%s %s", SNIPX_BUILD_DATE, SNIPX_BUILD_TIME);
    return buffer;
}

/**
 * 比较给定版本是否比当前编译版本更新。
 *
 * @return 给定版本更新时返回 true。
 */
inline bool IsVersionNewer(int major, int minor, int patch)
{
    if (major > SNIPX_VERSION_MAJOR) return true;
    if (major < SNIPX_VERSION_MAJOR) return false;

    if (minor > SNIPX_VERSION_MINOR) return true;
    if (minor < SNIPX_VERSION_MINOR) return false;

    return patch > SNIPX_VERSION_PATCH;
}

#endif // VERSION_H
