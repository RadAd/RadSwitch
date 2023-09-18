#include "Log.h"

#include <Windows.h>

#include "Format.h"

thread_local HRESULT g_radloghr = ERROR_SUCCESS;

typedef void (*LogFA)(LogLevel l, const char* msg, SrcLocA src);
typedef void (*LogFW)(LogLevel l, const wchar_t* msg, SrcLocW src);

namespace {
    static HWND g_hWndLog = NULL;
    static LPCSTR g_strLogCaptionA = nullptr;
    static LPCWSTR g_strLogCaptionW = nullptr;

    const char* AsStringA(LogLevel l)
    {
        switch (l)
        {
        case LOG_DEBUG:  return "DEBUG";
        case LOG_INFO:   return "INFO";
        case LOG_WARN:   return "WARN";
        case LOG_ERROR:  return "ERROR";
        case LOG_ASSERT: return "ASSERT";
        default:         return "UNKNOWN";
        }
    }

    const wchar_t* AsStringW(LogLevel l)
    {
        switch (l)
        {
        case LOG_DEBUG:  return L"DEBUG";
        case LOG_INFO:   return L"INFO";
        case LOG_WARN:   return L"WARN";
        case LOG_ERROR:  return L"ERROR";
        case LOG_ASSERT: return L"ASSERT";
        default:         return L"UNKNOWN";
        }
    }

    void LogOutputDebug(LogLevel l, const char* msg, SrcLocA src)
    {
        OutputDebugStringA(Format("%s: %s\n%s\n%s(%u)\n", AsStringA(l), msg, src.funcsig, src.file, src.line).c_str());
    }

    void LogOutputDebug(LogLevel l, const wchar_t* msg, SrcLocW src)
    {
        OutputDebugStringW(Format(L"%s: %s\n%s\n%s(%u)\n", AsStringW(l), msg, src.funcsig, src.file, src.line).c_str());
    }

    UINT GetIcon(LogLevel l)
    {
        switch (l)
        {
        case LOG_DEBUG:  return MB_ICONINFORMATION;
        case LOG_INFO:   return MB_ICONINFORMATION;
        case LOG_WARN:   return MB_ICONWARNING;
        case LOG_ERROR:  return MB_ICONERROR;
        case LOG_ASSERT: return MB_ICONERROR;
        default:         return MB_ICONERROR;
        }
    }

    void LogMessageBox(LogLevel l, const char* msg, SrcLocA src)
    {
        MessageBoxA(g_hWndLog, Format("%s: %s\n%s %s:%u", AsStringA(l), msg, src.funcsig, src.file, src.line).c_str(), g_strLogCaptionA, MB_OK | GetIcon(l));
    }

    void LogMessageBox(LogLevel l, const wchar_t* msg, SrcLocW src)
    {
        MessageBoxW(g_hWndLog, Format(L"%s: %s\n%s %s:%u", AsStringW(l), msg, src.funcsig, src.file, src.line).c_str(), g_strLogCaptionW, MB_OK | GetIcon(l));
    }
}

LogFA LogRegistryA[5][2] = {
#ifdef _DEBUG
    { LogOutputDebug },
#else
    { },
#endif
    { LogOutputDebug, LogMessageBox },
    { LogOutputDebug, LogMessageBox },
    { LogOutputDebug, LogMessageBox },
#ifdef _DEBUG
    { LogOutputDebug, LogMessageBox },
#else
    { },
#endif
};

LogFW LogRegistryW[5][2] = {
#ifdef _DEBUG
    { LogOutputDebug },
#else
    { },
#endif
    { LogOutputDebug, LogMessageBox },
    { LogOutputDebug, LogMessageBox },
    { LogOutputDebug, LogMessageBox },
#ifdef _DEBUG
    { LogOutputDebug, LogMessageBox },
#else
    { },
#endif
};

extern "C" {

    void RadLogInitWnd(HWND hWndLog, LPCSTR strLogCaptionA, LPCWSTR strLogCaptionW)
    {
        g_hWndLog = hWndLog;
        g_strLogCaptionA = strLogCaptionA;
        g_strLogCaptionW = strLogCaptionW;
    }

    void RadLogA(LogLevel l, const char* msg, SrcLocA src)
    {
        for (LogFA f : LogRegistryA[l])
        {
            if (f) f(l, msg, src);
        }
    }

    void RadLogW(LogLevel l, const wchar_t* msg, SrcLocW src)
    {
        for (LogFW f : LogRegistryW[l])
        {
            if (f) f(l, msg, src);
        }
    }

}
