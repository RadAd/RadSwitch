#pragma once
#include <Windows.h>
#include <string>

#include "Convert.h"

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

#include <system_error>

// bug in std::system_category().message() in MSVC v143
// See https://github.com/microsoft/STL/issues/3254

struct win32_error_code
{
    explicit win32_error_code(DWORD e) noexcept : error(e) {}
    DWORD error;
};

namespace std
{
    template <>
    struct is_error_code_enum<win32_error_code> : std::true_type {};
}

class win32_error_category : public std::error_category
{
public:
    constexpr win32_error_category() noexcept : std::error_category(_Generic_addr) {}

    _NODISCARD const char* name() const noexcept override
    {
        return "win32";
    }

    _NODISCARD std::string message(int errcode) const override
    {
        return WinError::getMessage(DWORD(errcode), LPCSTR(nullptr), LPCSTR(nullptr));
    }
};

_NODISCARD std::error_category& win32_category() noexcept;

_NODISCARD inline std::error_code make_error_code(const win32_error_code& wec)
{
    return std::error_code(static_cast<int>(wec.error), win32_category());
}

_NODISCARD inline std::error_code make_win32_error_code(DWORD ec = GetLastError()) noexcept
{
    return std::error_code(win32_error_code(ec));
}

[[noreturn]] inline void throw_win32_error(DWORD ec = GetLastError())
{
    throw std::system_error(win32_error_code(ec));
}

[[noreturn]] inline void throw_win32_error(DWORD ec, const std::string& msg)
{
    throw std::system_error(win32_error_code(ec), msg);
}

[[noreturn]] inline void throw_win32_error(DWORD ec, const std::wstring& msg)
{
    throw_win32_error(ec, w2a(msg));
}

[[noreturn]] inline void throw_win32_error(DWORD ec, const char* msg)
{
    throw std::system_error(win32_error_code(ec), msg);
}

[[noreturn]] inline void throw_win32_error(DWORD ec, const wchar_t* msg)
{
    throw_win32_error(ec, w2a(msg).c_str());
}

//#define CHECK_LE_THROW(x) if (!(x)) throw WinError({ GetLastError(), nullptr, TEXT(#x) })
#define CHECK_LE_THROW(x) if (!(x)) throw_win32_error(GetLastError(), TEXT(#x))
#define CHECK_HR_THROW(x) if (FAILED(g_radloghr = x)) throw_win32_error(g_radloghr, TEXT(#x))
extern thread_local HRESULT g_raderrorhr;
