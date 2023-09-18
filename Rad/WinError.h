#pragma once
#include <Windows.h>
#include <string>

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

struct WinError
{
    DWORD dwError;
    LPCTSTR szModule;
    std::tstring szContext;

    std::tstring getMessage() const
    {
        return getMessage(dwError, szModule, szContext.c_str());
    }

    static std::string getMessage(DWORD dwError, LPCSTR szModule, LPCSTR szContext);
    static std::wstring getMessage(DWORD dwError, LPCWSTR szModule, LPCWSTR szContext);
};

#define CHECK_LE_THROW(x) if (!(x)) throw WinError(GetLastError(), nullptr, TEXT(#x))
