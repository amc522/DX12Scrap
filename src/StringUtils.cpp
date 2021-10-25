#include "StringUtils.h"

#include <Windows.h>

namespace scrap
{
std::wstring Utf8ToWideString(std::string_view utf8String)
{
    int wideStringSize = MultiByteToWideChar(CP_UTF8, 0, utf8String.data(), (int)utf8String.size(), nullptr, 0);
    std::wstring wideString;
    wideString.resize(wideStringSize);

    MultiByteToWideChar(CP_UTF8, 0, utf8String.data(), (int)utf8String.size(), wideString.data(), wideStringSize);
    return wideString;
}
}