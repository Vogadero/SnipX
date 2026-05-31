// 跨平台适配层实现

#include "Platform.h"

const wchar_t* GetPlatformName()
{
    return SNIPX_PLATFORM_NAME;
}

bool IsFullFeaturePlatform()
{
#if defined(SNIPX_PLATFORM_WINDOWS)
    return true;
#else
    return false;
#endif
}
