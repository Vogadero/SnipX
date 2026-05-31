// 本地化文本管理实现

#include "Localization.h"

bool IsChineseLanguage()
{
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_CHINESE;
}

const WCHAR* L10n(const WCHAR* zh, const WCHAR* en)
{
    return IsChineseLanguage() ? zh : en;
}
