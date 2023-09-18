#pragma once
#include <Windows.h>

#include "SourceLocation.h"

#define CHECK(x) if (!(x)) RadLog(LOG_ASSERT, TEXT(#x), SRC_LOC)
#define CHECK_RET(x,r) if (!(x)) { RadLog(LOG_ASSERT, TEXT(#x), SRC_LOC); return (r); }

#ifdef __cplusplus
#include "WinError.h"
#define CHECK_LE(x) if (!(x)) RadLog(LOG_ASSERT, WinError::getMessage(GetLastError(), nullptr, TEXT(#x)), SRC_LOC)
#define CHECK_LE_RET(x, r) if (!(x)) { RadLog(LOG_ASSERT, WinError::getMessage(GetLastError(), nullptr, TEXT(#x)), SRC_LOC); return (r); }
#define CHECK_HR(x) if (FAILED(g_radloghr = x)) RadLog(LOG_ASSERT, WinError::getMessage(g_radloghr, nullptr, TEXT(#x)), SRC_LOC)
#define CHECK_HR_RET(x, r) if (FAILED(g_radloghr = x)) { RadLog(LOG_ASSERT, WinError::getMessage(g_radloghr, nullptr, TEXT(#x)), SRC_LOC); return (r); }
extern thread_local HRESULT g_radloghr;
#endif

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_ASSERT,
};

#ifdef __cplusplus
extern "C" {
#endif
    void RadLogInitWnd(HWND hWndLog, LPCSTR strLogCaptionA, LPCWSTR strLogCaptionW);
    void RadLogA(enum LogLevel l, const char* msg, SrcLocA src);
    void RadLogW(enum LogLevel l, const wchar_t* msg, SrcLocW src);
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#include <string>
inline void RadLog(LogLevel l, const char* msg, SrcLocA src) { RadLogA(l, msg, src); }
inline void RadLog(LogLevel l, const wchar_t* msg, SrcLocW src) { RadLogW(l, msg, src); }
inline void RadLog(LogLevel l, const std::string& msg, SrcLocA src) { RadLogA(l, msg.c_str(), src); }
inline void RadLog(LogLevel l, const std::wstring& msg, SrcLocW src) { RadLogW(l, msg.c_str(), src); }
#else
#ifdef _UNICODE
#define RadLog RadLogW
#else
#define RadLog RadLogA
#endif
#endif
