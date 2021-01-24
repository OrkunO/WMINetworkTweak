#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <locale>
#include <codecvt>
#include <string>

using namespace std;
class StringHelper
{
public:
    static wstring s2ws(const string& str);
    static string ws2s(const wstring& wstr);
};

#endif
