// 本地化文本管理

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <windows.h>

/**
 * 获取当前界面语言是否为中文。
 *
 * @return 当前用户默认 UI 语言为中文时返回 true。
 */
bool IsChineseLanguage();

/**
 * 根据当前系统语言返回中文或英文文本。
 *
 * @param zh 中文文本。
 * @param en 英文文本。
 * @return 当前语言对应的文本。
 */
const WCHAR* L10n(const WCHAR* zh, const WCHAR* en);

#endif // LOCALIZATION_H
