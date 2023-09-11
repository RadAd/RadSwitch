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

inline UINT MySendKey(WORD wVk, bool bUp)
{
    INPUT i = {};
    i.type = INPUT_KEYBOARD;
    i.ki.wVk = wVk;
    if (bUp)
        i.ki.dwFlags |= KEYEVENTF_KEYUP;
    return SendInput(1, &i, sizeof(i));
}

inline void SendHotKey(_In_ UINT fsModifiers, _In_ UINT vk)
{
    const bool bAltDown = GetKeyState(VK_MENU) < 0;
    const bool bAltCtrl = GetKeyState(VK_CONTROL) < 0;
    const bool bAltShift = GetKeyState(VK_SHIFT) < 0;

    if (!bAltDown && AllSet(fsModifiers, MOD_ALT))
        MySendKey(VK_MENU, false);
    if (!bAltCtrl && AllSet(fsModifiers, MOD_CONTROL))
        MySendKey(VK_CONTROL, false);
    if (!bAltShift && AllSet(fsModifiers, MOD_SHIFT))
        MySendKey(VK_SHIFT, false);

    MySendKey(vk, false);
    MySendKey(vk, true);

    if (!bAltShift && AllSet(fsModifiers, MOD_SHIFT))
        MySendKey(VK_SHIFT, true);
    if (!bAltCtrl && AllSet(fsModifiers, MOD_CONTROL))
        MySendKey(VK_CONTROL, true);
    if (!bAltDown && AllSet(fsModifiers, MOD_ALT))
        MySendKey(VK_MENU, true);
}

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
std::vector<HMONITOR> GetMonitors();
HMONITOR GetPrimaryMonitor(const std::vector<HMONITOR>& ms);

