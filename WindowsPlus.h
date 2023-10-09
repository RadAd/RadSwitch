#pragma once
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <vector>

inline bool AllSet(DWORD v, DWORD set)
{
    return (v & set) == set;
}

inline bool NoneSet(DWORD v, DWORD set)
{
    return (v & set) == 0;
}

void SendHotKey(_In_ UINT fsModifiers, _In_ UINT vk);

inline DWORD MySendMessageTimeout(
    _In_ HWND hWnd,
    _In_ UINT Msg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ UINT fuFlags,
    _In_ UINT uTimeout)
{
    DWORD_PTR dwResult = 0;
    SendMessageTimeout(
        hWnd,
        Msg,
        wParam,
        lParam,
        fuFlags,
        uTimeout,
        &dwResult);
    return static_cast<DWORD>(dwResult);
}

inline LONG Width(RECT r)
{
    return r.right - r.left;
}

inline LONG Height(RECT r)
{
    return r.bottom - r.top;
}

void SetWindowBlur(HWND hWnd);
std::vector<HWND> GetWindows();
std::vector<HWND> GetWindows(HWND hWndParent);
std::vector<HMONITOR> GetMonitors();
HMONITOR GetPrimaryMonitor(const std::vector<HMONITOR>& ms);

