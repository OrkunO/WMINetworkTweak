#include "StringHelper.h"

using convert_type = codecvt_utf8<wchar_t>;
wstring_convert<convert_type, wchar_t> converter;

wstring StringHelper::s2ws(const string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

string StringHelper::ws2s(const wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}
