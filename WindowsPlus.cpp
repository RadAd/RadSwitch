#include "WindowsPlus.h"

namespace {

    inline UINT MySendKey(WORD wVk, bool bUp)
    {
        INPUT i = {};
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = wVk;
        if (bUp)
            i.ki.dwFlags |= KEYEVENTF_KEYUP;
        return SendInput(1, &i, sizeof(i));
    }

}

void SendHotKey(_In_ UINT fsModifiers, _In_ UINT vk)
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

void SetWindowBlur(HWND hWnd)
{
    const HINSTANCE hModule = GetModuleHandle(TEXT("user32.dll"));
    if (hModule)
    {
        typedef enum _ACCENT_STATE {
            ACCENT_DISABLED,
            ACCENT_ENABLE_GRADIENT,
            ACCENT_ENABLE_TRANSPARENTGRADIENT,
            ACCENT_ENABLE_BLURBEHIND,
            ACCENT_ENABLE_ACRYLICBLURBEHIND,
            ACCENT_INVALID_STATE
        } ACCENT_STATE;
        struct ACCENTPOLICY
        {
            ACCENT_STATE nAccentState;
            DWORD nFlags;
            DWORD nColor;
            DWORD nAnimationId;
        };
        typedef enum _WINDOWCOMPOSITIONATTRIB {
            WCA_UNDEFINED = 0,
            WCA_NCRENDERING_ENABLED = 1,
            WCA_NCRENDERING_POLICY = 2,
            WCA_TRANSITIONS_FORCEDISABLED = 3,
            WCA_ALLOW_NCPAINT = 4,
            WCA_CAPTION_BUTTON_BOUNDS = 5,
            WCA_NONCLIENT_RTL_LAYOUT = 6,
            WCA_FORCE_ICONIC_REPRESENTATION = 7,
            WCA_EXTENDED_FRAME_BOUNDS = 8,
            WCA_HAS_ICONIC_BITMAP = 9,
            WCA_THEME_ATTRIBUTES = 10,
            WCA_NCRENDERING_EXILED = 11,
            WCA_NCADORNMENTINFO = 12,
            WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
            WCA_VIDEO_OVERLAY_ACTIVE = 14,
            WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
            WCA_DISALLOW_PEEK = 16,
            WCA_CLOAK = 17,
            WCA_CLOAKED = 18,
            WCA_ACCENT_POLICY = 19,
            WCA_FREEZE_REPRESENTATION = 20,
            WCA_EVER_UNCLOAKED = 21,
            WCA_VISUAL_OWNER = 22,
            WCA_LAST = 23
        } WINDOWCOMPOSITIONATTRIB;
        struct WINCOMPATTRDATA
        {
            WINDOWCOMPOSITIONATTRIB nAttribute;
            PVOID pData;
            ULONG ulDataSize;
        };
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
        const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute) GetProcAddress(hModule, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute)
        {
            ACCENTPOLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, 0x80000000, 0 };
            //ACCENTPOLICY policy = { ACCENT_ENABLE_BLURBEHIND };
            WINCOMPATTRDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENTPOLICY) };
            SetWindowCompositionAttribute(hWnd, &data);
            //DwmSetWindowAttribute(hWnd, WCA_ACCENT_POLICY, &policy, sizeof(ACCENTPOLICY));
        }
        //FreeLibrary(hModule);
    }
}

static BOOL CALLBACK MyEnumWindows(HWND hWnd, LPARAM lParam)
{
    std::vector<HWND>& w(*reinterpret_cast<std::vector<HWND>*>(lParam));
    w.push_back(hWnd);
    return TRUE;
}

std::vector<HWND> GetWindows()
{
    std::vector<HWND> w;
    EnumWindows(MyEnumWindows, reinterpret_cast<LPARAM>(&w));
    return w;
}

std::vector<HWND> GetWindows(HWND hWndParent)
{
    std::vector<HWND> w;
    EnumChildWindows(hWndParent, MyEnumWindows, reinterpret_cast<LPARAM>(&w));
    return w;
}

static BOOL CALLBACK MyEnumMonitors(HMONITOR hMonitor, HDC hDC, LPRECT pRect, LPARAM lParam)
{
    std::vector<HMONITOR>& w(*reinterpret_cast<std::vector<HMONITOR>*>(lParam));
    w.push_back(hMonitor);
    return TRUE;
}

std::vector<HMONITOR> GetMonitors()
{
    std::vector<HMONITOR> m;
    EnumDisplayMonitors(NULL, nullptr, MyEnumMonitors, reinterpret_cast<LPARAM>(&m));
    return m;
}

HMONITOR GetPrimaryMonitor(const std::vector<HMONITOR>& ms)
{
    for (HMONITOR m : ms)
    {
        MONITORINFO mi = { sizeof(MONITORINFO) };
        GetMonitorInfo(m, &mi);
        if (mi.dwFlags & MONITORINFOF_PRIMARY)
            return m;
    }
    return ms.front();
}
