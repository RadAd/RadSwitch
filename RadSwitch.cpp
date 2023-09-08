#include "Window.h"
#include "Windowxx.h"
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <dwmapi.h>
#include <appmodel.h>
#include <algorithm>
#include <vector>
//#include <tchar.h>

#include "resource.h"

#define HK_1_MOD    (MOD_ALT | MOD_CONTROL | MOD_SHIFT)
#define HK_1_KEY    (VK_OEM_FJ_TOUROKU)
#define HK_1    (100)
#define HK_2_MOD    (MOD_ALT | MOD_CONTROL | MOD_SHIFT)
#define HK_2_KEY    (VK_OEM_FJ_MASSHOU)
#define HK_2    (200)

static HHOOK g_hHook{ NULL };
static HWND g_hWnd{ NULL };

#define WM_START (WM_USER + 124)
#define WM_STOP  (WM_USER + 532)

#define LIST_ID 101

bool IsTaskSwitcher(HWND hWnd)
{
    TCHAR strClass[1024];
    GetClassName(hWnd, strClass, ARRAYSIZE(strClass));
    return lstrcmp(strClass, TEXT("MultitaskingViewFrame")) == 0;
}

inline bool AllSet(DWORD v, DWORD set)
{
    return (v & set) == set;
}

inline bool NoneSet(DWORD v, DWORD set)
{
    return (v & set) == 0;
}

UINT MySendKey(WORD wVk, bool bUp)
{
    INPUT i = {};
    i.type = INPUT_KEYBOARD;
    i.ki.wVk = wVk;
    if (bUp)
        i.ki.dwFlags |= KEYEVENTF_KEYUP;
    return SendInput(1, &i, sizeof(i));
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

inline LONG Width(RECT r)
{
    return r.right - r.left;
}

inline LONG Height(RECT r)
{
    return r.bottom - r.top;
}

// https://devblogs.microsoft.com/oldnewthing/20071008-00/?p=24863
BOOL IsAltTabWindow(HWND hwnd)
{
    // Start at the root owner
    HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);
    // See if we are the last active visible popup
    HWND hwndTry;
    while ((hwndTry = GetLastActivePopup(hwndWalk)) != hwndWalk)
    {
        hwndWalk = hwndTry;
        if (IsWindowVisible(hwndTry))
            break;
    }
    return hwndWalk == hwnd;
}

void ShowLastError(LPCTSTR msg, LPCTSTR function)
{
    TCHAR fullmsg[1024];
    wsprintf(fullmsg, TEXT("%s - Failed in %s"), msg, function);
    MessageBox(g_hWnd, fullmsg, TEXT("Rad Switch"), MB_OK | MB_ICONERROR);
}

#define CHECK(x) if (!(x)) ShowLastError(TEXT(#x), TEXT(__FUNCTION__));

void ForceForegroundWindow(HWND hWnd)
{
    DWORD windowThreadProcessId = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    DWORD currentThreadId = GetCurrentThreadId();
    if (AttachThreadInput(windowThreadProcessId, currentThreadId, true))
    {
        CHECK(BringWindowToTop(hWnd));
        ShowWindow(hWnd, SW_SHOW);
        CHECK(SetForegroundWindow(hWnd));
        AttachThreadInput(windowThreadProcessId, currentThreadId, false);
    }
    else
    {
        DWORD timeout = 0;
        DWORD zero = 0;
        SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &timeout, 0);
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &zero, 0);
        CHECK(BringWindowToTop(hWnd));
        ShowWindow(hWnd, SW_SHOW);
        SetForegroundWindow(hWnd);
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &timeout, 0);
    }
}

struct Theme
{
    COLORREF clrWindow = GetSysColor(COLOR_WINDOW);
    COLORREF clrHighlight = GetSysColor(COLOR_HIGHLIGHT);
    COLORREF clrWindowText = GetSysColor(COLOR_WINDOWTEXT);
    COLORREF clrHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
    HBRUSH   brWindow = NULL;
};

Theme g_Theme;

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

RECT CalcSizeListBox(HWND hWndLB, LONG max)
{
    const int iItemHeight = ListBox_GetItemHeight(hWndLB, 0);
    const int iItemCount = ListBox_GetCount(hWndLB);

    RECT rc;
    GetClientRect(hWndLB, &rc);
    rc.bottom = rc.top + iItemHeight * std::min<LONG>(iItemCount, max / iItemHeight);

    AdjustWindowRect(&rc, GetWindowStyle(hWndLB), FALSE);

    return rc;
}

inline int ListBox_WrapIndex(HWND hWnd, int i)
{
    const int count = ListBox_GetCount(hWnd);
    if (i < 0)
        i += count;
    if (i >= count)
        i -= count;
    return i;
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

HANDLE GetProcessHandleFromHwnd(HWND hWnd)
{
    DWORD pid = 0;
    if (GetWindowThreadProcessId(hWnd, &pid) == 0)
        return NULL;

    return OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
}

BOOL QueryFullProcessImageNameFromHwnd(HWND hWnd, _Out_writes_to_(dwSize, dwSize) LPTSTR lpExeName, _Inout_ DWORD dwSize)
{
    lpExeName[0] = TEXT('\0');

    HANDLE hProcess = GetProcessHandleFromHwnd(hWnd);
    if (hProcess == NULL)
        return FALSE;

    QueryFullProcessImageName(hProcess, 0, lpExeName, &dwSize);

    CloseHandle(hProcess);
    
    return TRUE;
}

void GetProductName(LPCTSTR strFileName, LPTSTR lpszBuffer)
{
    DWORD	Dummy = 0;
    DWORD	Size = GetFileVersionInfoSize(strFileName, &Dummy);
    if (Size <= 0)
        return;

    void* Info = malloc(Size);
    if (Info == nullptr)
        return;

    GetFileVersionInfoEx(FILE_VER_GET_LOCALISED, strFileName, 0, Size, Info);

    struct LANGANDCODEPAGE
    {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    UINT	cbTranslate;
    VerQueryValue(Info, TEXT("\\VarFileInfo\\Translation"), (LPVOID*) &lpTranslate, &cbTranslate);

    for (int i = 0; i < (int) (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
    {
        TCHAR SubBlock[50];
        TCHAR* String;
        UINT	Length;

        String = nullptr;
        //_stprintf_s(SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileDescription"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
        swprintf_s(SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileDescription"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
        VerQueryValue(Info, SubBlock, (LPVOID*) &String, &Length);
        if (String != nullptr)
        {
            //_tcscpy_s(lpszBuffer, String);
            lstrcpy(lpszBuffer, String);
            break;
        }
    }

    free(Info);
}


static LRESULT CALLBACK KeyboardllHook(const int nCode, const WPARAM wParam, const LPARAM lParam)
{
    if (nCode >= 0 && g_hWnd != NULL)
    {
        const KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*) lParam;

        switch (wParam)
        {
        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            if (GetKeyState(VK_MENU) < 0 && !IsTaskSwitcher(GetForegroundWindow()))
            {
                if (GetForegroundWindow() == g_hWnd)
                {
                    PostMessage(GetFocus(), WM_KEYDOWN, kbdStruct->vkCode, 0); // TODO Set lParam
                    if (kbdStruct->vkCode == VK_TAB || kbdStruct->vkCode == VK_ESCAPE)
                        return 1;
                }
                else switch (kbdStruct->vkCode)
                {
                case VK_TAB:
                    //SendMessage(g_hWnd, WM_START, FALSE, 0);
                    SendHotKey(HK_1_MOD, HK_1_KEY);
                    return 1;
                case VK_OEM_3: // Backtick
                    //PostMessage(g_hWnd, WM_START, TRUE, 0);
                    SendHotKey(HK_2_MOD, HK_2_KEY);
                    return 1;
                }
            }
            break;
        case WM_KEYUP: case WM_SYSKEYUP:
            switch (kbdStruct->vkCode)
            {
            case VK_LMENU: case VK_RMENU:
            {
                if (GetForegroundWindow() == g_hWnd)
                {
                    PostMessage(g_hWnd, WM_STOP, 0, 0);
                    //return 1;
                }
            }
            break;
            }
            break;
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
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

#if 0
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
#endif

RECT GetPosition(HMONITOR hMonitor)
{
    MONITORINFO MonitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &MonitorInfo);
    RECT r(MonitorInfo.rcMonitor);
    InflateRect(&r, Width(MonitorInfo.rcMonitor) / -4, Height(MonitorInfo.rcMonitor) / -3);
    return r;
}

class ListBox
{
public:
    void Create(_In_ HWND hParent, _In_ DWORD dwStyle, _In_ RECT rPos, _In_ int nID)
    {
        const HINSTANCE hInstance = NULL;
        m_hWnd = CreateWindow(WC_LISTBOX, nullptr, dwStyle, rPos.left, rPos.top, Width(rPos), Height(rPos), hParent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(nID)), hInstance, NULL);
    }

    operator bool() const { return m_hWnd != NULL; }

    operator HWND() const { return m_hWnd; }

    void ResetContent() { ListBox_ResetContent(m_hWnd); }
    int AddString(LPCTSTR lpsz) { return ListBox_AddString(m_hWnd, lpsz); }
    int GetCount() const { return ListBox_GetCount(m_hWnd); }
    int GetCurSel() const { return ListBox_GetCurSel(m_hWnd); }
    LPARAM GetItemData(int i) const { return ListBox_GetItemData(m_hWnd, i); }
    int GetItemRect(int i, LPRECT pRect) { return ListBox_GetItemRect(m_hWnd, i , pRect); }
    int GetText(int i, LPTSTR lpszBuffer) const { return ListBox_GetText(m_hWnd, i, lpszBuffer); }
    int SetCurSel(int i) { return ListBox_SetCurSel(m_hWnd, i); }
    int SetItemData(int i, LPARAM data) { return ListBox_SetItemData(m_hWnd, i, data); }
    int SetTopIndex(int i) { return ListBox_SetTopIndex(m_hWnd, i); }

private:
    HWND m_hWnd{ NULL };
};

class ListBoxOwnerDrawnFixed : public ListBox
{
private:
    struct ItemData
    {
        LPARAM lParam;
        HICON hIcon;
        TCHAR strRight[MAX_PATH];
    };

    ItemData* GetInternalItemData(int i)
    {
        const LPARAM lParam = ListBox::GetItemData(i);
        ItemData* pData = lParam == CB_ERR ? nullptr : reinterpret_cast<ItemData*>(lParam);
        if (pData == nullptr)
        {
            pData = new ItemData({});
            ListBox::SetItemData(i, reinterpret_cast<LPARAM>(pData));
        }
        return pData;
    }

    const ItemData* GetInternalItemData(int i) const
    {
        const LPARAM lParam = ListBox::GetItemData(i);
        ItemData* pData = lParam == CB_ERR ? nullptr : reinterpret_cast<ItemData*>(lParam);
        return pData;
    }

public:
    void Create(_In_ HWND hParent, _In_ DWORD dwStyle, _In_ RECT rPos, _In_ int nID)
    {
        m_nID = nID;
        ListBox::Create(hParent, dwStyle | LBS_OWNERDRAWFIXED, rPos, nID);
    }

    LPARAM GetItemData(int i) const
    {
        const ItemData* pData = GetInternalItemData(i);
        return pData ? pData->lParam : 0;
    }

    void SetItemData(int i, LPARAM data)
    {
        ItemData* pData = GetInternalItemData(i);
        pData->lParam = data;
    }

    void SetItemIcon(int i, HICON hIcon)
    {
        ItemData* pData = GetInternalItemData(i);
        pData->hIcon = hIcon;
        InvalidateItem(i);
    }

    void SetItemRightString(int i, LPCTSTR s)
    {
        ItemData* pData = GetInternalItemData(i);
        lstrcpy(pData->strRight, s);
        InvalidateItem(i);
    }

    void InvalidateItem(int i)
    {
        if (IsWindow(*this))
        {
            RECT r;
            GetItemRect(i, &r);
            InvalidateRect(*this, &r, FALSE);
        }
    }

private:
    void OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItem)
    {
        //if (lpMeasureItem->CtlID == GetWindowLong(m_hWndChild, GWL_ID))
        if (lpMeasureItem->CtlID == m_nID)
        {
            lpMeasureItem->itemHeight = GetSystemMetrics(SM_CYICON) + 4;
        }
    }

    void OnDrawItem(const DRAWITEMSTRUCT* lpDrawItem)
    {
        if (lpDrawItem->hwndItem == *this)
        {
            if (lpDrawItem->itemID == -1) // Empty item
                return;

            const ItemData* pData = reinterpret_cast<ItemData*>(lpDrawItem->itemData);

            const COLORREF clrForeground = SetTextColor(lpDrawItem->hDC,
                lpDrawItem->itemState & ODS_SELECTED ? g_Theme.clrHighlightText : g_Theme.clrWindowText);

            const COLORREF clrBackground = SetBkColor(lpDrawItem->hDC,
                lpDrawItem->itemState & ODS_SELECTED ? g_Theme.clrHighlight : g_Theme.clrWindow);

            TEXTMETRIC tm;
            GetTextMetrics(lpDrawItem->hDC, &tm);
            const int y = (lpDrawItem->rcItem.bottom + lpDrawItem->rcItem.top - tm.tmHeight) / 2;
            const int x = GetSystemMetrics(SM_CXICON) + 4;

            TCHAR text[1024];
            const int cch = GetText(lpDrawItem->itemID, text);

            const UINT align = SetTextAlign(lpDrawItem->hDC, TA_RIGHT);
            ExtTextOut(lpDrawItem->hDC, lpDrawItem->rcItem.right - 2, y,
                ETO_CLIPPED | ETO_OPAQUE, &lpDrawItem->rcItem,
                pData->strRight, lstrlen(pData->strRight), NULL);
            SetTextAlign(lpDrawItem->hDC, align);

            ExtTextOut(lpDrawItem->hDC, x, y,
                ETO_CLIPPED, &lpDrawItem->rcItem,
                text, (UINT) cch, NULL);

            SetTextColor(lpDrawItem->hDC, clrForeground);
            SetBkColor(lpDrawItem->hDC, clrBackground);

            DrawIcon(lpDrawItem->hDC, lpDrawItem->rcItem.left + 2, lpDrawItem->rcItem.top + 2, pData->hIcon);

            if (lpDrawItem->itemState & ODS_FOCUS)
                DrawFocusRect(lpDrawItem->hDC, &lpDrawItem->rcItem);
        }
    }

    void OnDeleteItem(const DELETEITEMSTRUCT* lpDeleteItem)
    {
        if (lpDeleteItem->hwndItem == *this)
        {
            ItemData* pData = reinterpret_cast<ItemData*>(lpDeleteItem->itemData);
            delete pData;
        }
    }

public:
    LRESULT HandleMessage(const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
    {
        LRESULT ret = 0;
        switch (uMsg)
        {
            HANDLE_MSG(WM_MEASUREITEM, OnMeasureItem);
            HANDLE_MSG(WM_DRAWITEM, OnDrawItem);
            HANDLE_MSG(WM_DELETEITEM, OnDeleteItem);
        }

        return ret;
    }

private:
    int m_nID{ 0 };
};

class RootWindow : public Window
{
    friend WindowManager<RootWindow>;

public:
    static bool IsExisting() { return FindWindow(ClassName(), nullptr) != NULL; }
    static ATOM Register() { return WindowManager<RootWindow>::Register(); }
    static RootWindow* Create() { return WindowManager<RootWindow>::Create(); }

protected:
    static void GetCreateWindow(CREATESTRUCT& cs);
    static void GetWndClass(WNDCLASS& wc);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnSize(UINT state, int cx, int cy);
    void OnSetFocus(HWND hwndOldFocus);
    int OnVkeyToItem(UINT vk, HWND hwndListbox, int iCaret);
    void OnHotKey(int idHotKey, UINT fuModifiers, UINT vk);
    void OnActivate(UINT state, HWND hwndActDeact, BOOL fMinimized);

    static LPCTSTR ClassName() { return TEXT("RadSwitch"); }

    void FillList(HWND hActiveWnd, LPCTSTR lpExeActive);
    void Switch(int iCaret);

    ListBoxOwnerDrawnFixed m_hWndChild;
};

void RootWindow::GetCreateWindow(CREATESTRUCT& cs)
{
    Window::GetCreateWindow(cs);
    cs.lpszName = TEXT("Rad Switch");
    cs.style = WS_POPUP | WS_BORDER;
    cs.dwExStyle |= WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
}

void RootWindow::GetWndClass(WNDCLASS& wc)
{
    Window::GetWndClass(wc);
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hbrBackground = g_Theme.brWindow;
}

BOOL RootWindow::OnCreate(const LPCREATESTRUCT lpCreateStruct)
{
    SetWindowBlur(*this);

    RECT r;
    GetWindowRect(*this, &r);
    m_hWndChild.Create(*this, WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_HASSTRINGS | LBS_WANTKEYBOARDINPUT | LBS_OWNERDRAWFIXED, r, LIST_ID);

    CHECK(RegisterHotKey(*this, HK_1, HK_1_MOD, HK_1_KEY));
    CHECK(RegisterHotKey(*this, HK_2, HK_2_MOD, HK_2_KEY));

    g_hWnd = *this;
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardllHook, NULL, 0);
    //g_hHook2 = SetWindowsHookEx(WH_SHELL, ShellHook, NULL, 0); // TODO

    return TRUE;
}

void RootWindow::OnDestroy()
{
    PostQuitMessage(0);

    UnhookWindowsHookEx(g_hHook);
    g_hWnd = NULL;
}

void RootWindow::OnSize(const UINT state, const int cx, const int cy)
{
    if (m_hWndChild)
    {
        SetWindowPos(m_hWndChild, NULL,
            0, 0, cx, cy,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void RootWindow::OnSetFocus(const HWND hWndOldFocus)
{
    if (m_hWndChild)
        SetFocus(m_hWndChild);
}

int RootWindow::OnVkeyToItem(UINT vk, HWND hWndListbox, int iCaret)
{
    if (hWndListbox == m_hWndChild)
    {
        switch (vk)
        {
        case VK_TAB: case VK_OEM_3:
            m_hWndChild.SetCurSel(ListBox_WrapIndex(m_hWndChild, iCaret + (GetKeyState(VK_SHIFT) < 0 ? -1 : +1)));
            return -2;
        case VK_ESCAPE:
            ShowWindow(*this, SW_HIDE);
            return -2;
        case VK_RETURN: case VK_SPACE:
            Switch(iCaret);
            return -2;
        default:
            return -1;
        }
    }
    else
        return -1;
}

void RootWindow::OnHotKey(int idHotKey, UINT fuModifiers, UINT vk)
{
    switch (idHotKey)
    {
    case HK_1: if (GetForegroundWindow() != *this) PostMessage(g_hWnd, WM_START, FALSE, 0); break;
    case HK_2: if (GetForegroundWindow() != *this) PostMessage(g_hWnd, WM_START, TRUE, 0); break;
    }
}

void RootWindow::OnActivate(UINT state, HWND hwndActDeact, BOOL fMinimized)
{
    if (!state)
        ShowWindow(*this, SW_HIDE);
}

LRESULT RootWindow::HandleMessage(const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    LRESULT ret = 0;
    switch (uMsg)
    {
        HANDLE_MSG(WM_CREATE, OnCreate);
        HANDLE_MSG(WM_DESTROY, OnDestroy);
        HANDLE_MSG(WM_SIZE, OnSize);
        HANDLE_MSG(WM_SETFOCUS, OnSetFocus);
        HANDLE_MSG(WM_VKEYTOITEM, OnVkeyToItem);
        HANDLE_MSG(WM_HOTKEY, OnHotKey);
        HANDLE_MSG(WM_ACTIVATE, OnActivate);
    case WM_START:
    {
        HWND hActiveWnd = GetForegroundWindow();
        TCHAR strExe[MAX_PATH]{ TEXT("") };
        if (!wParam || !QueryFullProcessImageNameFromHwnd(hActiveWnd, strExe, ARRAYSIZE(strExe)))
            strExe[0] = TEXT('\0');

        FillList(hActiveWnd, strExe);
        if (m_hWndChild.GetCount() > 0) // TODO Maybe > 1
        {
            if (true)
            {
                RECT rcWnd(GetPosition(MonitorFromWindow(hActiveWnd, MONITOR_DEFAULTTOPRIMARY)));
                const RECT rcList = CalcSizeListBox(m_hWndChild, Height(rcWnd));
                rcWnd.bottom = rcWnd.top + Height(rcList);
                AdjustWindowRect(&rcWnd, GetWindowStyle(*this), FALSE);
                SetWindowPos(*this, NULL,
                    rcWnd.left, rcWnd.top, Width(rcWnd), Height(rcWnd),
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }
#if 1
            ShowWindow(*this, SW_SHOW);
            CHECK(BringWindowToTop(*this));
            CHECK(SetForegroundWindow(*this));
#else
            ForceForegroundWindow(*this);
#endif
        }
    }
        break;
    case WM_STOP:
        Switch(m_hWndChild.GetCurSel());
        break;
    }

    if (!IsHandled())
        ret = m_hWndChild.HandleMessage(uMsg, wParam, lParam);
    if (!IsHandled())
        ret = Window::HandleMessage(uMsg, wParam, lParam);

    switch (uMsg)
    {
    case WM_CTLCOLORLISTBOX:
        ret = (LRESULT) g_Theme.brWindow;
        break;
    }

    return ret;
}

void RootWindow::FillList(HWND hActiveWnd, LPCTSTR lpExeActive)
{
    const std::vector<HWND> w = GetWindows();

    int selected = -1;
    m_hWndChild.ResetContent();
    for (const HWND hWnd : w)
    {
        if (hWnd != *this)
        {
            // TODO Filter to this Virtual Desktop
            // TODO Filter to this Monitor
            UINT CloakedVal;
            DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &CloakedVal, sizeof(CloakedVal));

            const DWORD dwStyle = GetWindowStyle(hWnd);
            const DWORD dwExStyle = GetWindowExStyle(hWnd);
            if (IsWindowVisible(hWnd) && (CloakedVal == 0) && NoneSet(dwExStyle, WS_EX_TOOLWINDOW) && IsAltTabWindow(hWnd))
            {
                IsAltTabWindow(hWnd);
                TCHAR title[1024];
                GetWindowText(hWnd, title, ARRAYSIZE(title));
                if (IsMinimized(hWnd))
                    lstrcat(title, TEXT("*"));

                TCHAR strExe[MAX_PATH] = TEXT("");
                QueryFullProcessImageNameFromHwnd(hWnd, strExe, ARRAYSIZE(strExe));

                if (lpExeActive[0] != TEXT('\0') && lstrcmp(lpExeActive, strExe) != 0)
                    continue;

                TCHAR strProduct[1024] = TEXT("");
                GetProductName(strExe, strProduct);

                HICON hIcon = NULL;
                if (hIcon == NULL)
                    hIcon = reinterpret_cast<HICON>(static_cast<INT_PTR>(MySendMessageTimeout(hWnd, WM_GETICON, ICON_BIG, GetSystemMetrics(SM_CXICON), SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 100)));
                if (hIcon == NULL)
                    hIcon = reinterpret_cast<HICON>(GetClassLongPtr(hWnd, GCLP_HICON));
                if (hIcon == NULL)
                    hIcon = LoadIcon(NULL, IDI_APPLICATION);
                // TODO Get an icon for UWP apps

#if 0 // UWP test
                HANDLE hProcess = GetProcessHandleFromHwnd(hWnd);
                WCHAR name[1024];
                UINT32 length = ARRAYSIZE(name);
                if (GetPackageFullName(hProcess, &length, name) == ERROR_SUCCESS)
                {
                    length = length;
                }
#endif

                const int i = m_hWndChild.AddString(title);
                m_hWndChild.SetItemIcon(i, hIcon);
                if (strProduct[0] != TEXT('\0'))
                    m_hWndChild.SetItemRightString(i, strProduct);
                else
                    m_hWndChild.SetItemRightString(i, PathFindFileName(strExe));
                m_hWndChild.SetItemData(i, reinterpret_cast<LPARAM>(hWnd));
                if (hActiveWnd == hWnd || hActiveWnd == GetLastActivePopup(hWnd))
                    selected = i;
            }
        }
    }
    m_hWndChild.SetTopIndex(0);
    m_hWndChild.SetCurSel(ListBox_WrapIndex(m_hWndChild, selected + (GetKeyState(VK_SHIFT) < 0 ? -1 : +1)));
}

void RootWindow::Switch(int iCaret)
{
    HWND hWnd = reinterpret_cast<HWND>(m_hWndChild.GetItemData(iCaret));
#if 0
    if (IsMinimized(hWnd))
        ShowWindow(hWnd, SW_NORMAL);
    HWND hWndPopup = GetLastActivePopup(hWnd);
    SetForegroundWindow(hWndPopup);
#else
    SwitchToThisWindow(GetLastActivePopup(hWnd), TRUE);
#endif
    //ShowWindow(*this, SW_HIDE);
}

bool Run(_In_ const LPCTSTR lpCmdLine, _In_ const int nShowCmd)
{
    if (RootWindow::IsExisting())
    {
        MessageBox(NULL, TEXT("Process already exists."), TEXT("Rad Switch"), MB_ICONERROR | MB_OK);
        return false;
    }

    if (true)
    {
        g_Theme.clrWindow = RGB(0, 0, 0);
        g_Theme.clrHighlight = RGB(61, 61, 61);
        g_Theme.clrWindowText = RGB(250, 250, 250);
        g_Theme.clrHighlightText = g_Theme.clrWindowText;
    }

    g_Theme.brWindow = CreateSolidBrush(g_Theme.clrWindow);

    if (RootWindow::Register() == 0)
    {
        MessageBox(NULL, TEXT("Error registering window class"), TEXT("Rad Switch"), MB_ICONERROR | MB_OK);
        return false;
    }

    RootWindow* prw = RootWindow::Create();
    if (prw == nullptr)
    {
        MessageBox(NULL, TEXT("Error creating root window"), TEXT("Rad Switch"), MB_ICONERROR | MB_OK);
        return false;
    }

    // Needed to allow to capture foreground
    //ShowWindow(*prw, nShowCmd);
    //ShowWindow(*prw, SW_HIDE);
    return true;
}
