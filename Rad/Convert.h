#pragma once
#include <string>

inline std::string w2a(const wchar_t* sw)
{
    size_t sz;
    wcstombs_s(&sz, nullptr, 0, sw, 0);
    // TODO Throw on error?
    std::string s(sz - 1, '-');
    wcstombs_s(nullptr, const_cast<char*>(s.data()), sz, sw, sz);
    return s;
}

inline std::string w2a(const std::wstring& sw)
{
    return w2a(sw.c_str());
}

inline std::wstring a2w(const char* sa)
{
    size_t sz;
    mbstowcs_s(&sz, nullptr, 0, sa, 0);
    // TODO Throw on error?
    std::wstring s(sz - 1, '-');
    mbstowcs_s(nullptr, const_cast<wchar_t*>(s.data()), sz, sa, sz);
    return s;
}

inline std::wstring a2w(const std::string& sa)
{
    return a2w(sa.c_str());
}

#ifdef _UNICODE
inline std::wstring s2t(const char* sa) { return a2w(sa); }
inline std::wstring s2t(const std::string& sa) { return a2w(sa); }
inline std::wstring s2t(const wchar_t* sw) { return sw; }
inline std::wstring s2t(const std::wstring& sw) { return sw; }
#else
inline std::string s2t(const char* sa) { return sa; }
inline std::string s2t(const std::string& sa) { return sa; }
inline std::string s2t(const wchar_t* sw) { return w2a(sw); }
inline std::string s2t(const std::wstring& sw) { return w2a(sw); }
#endif
