#include "Window.h"
#include "Windowxx.h"
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <dwmapi.h>
#include <appmodel.h>
#include <algorithm>
#include <vector>
#include <iterator>
//#include <tchar.h>

#include "WindowsPlus.h"
#include "ListBoxPlus.h"

#include "resource.h"

Theme g_Theme;

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

void ShowLastError(LPCTSTR msg, LPCTSTR function)
{
    TCHAR fullmsg[1024];
    wsprintf(fullmsg, TEXT("%s - Failed in %s"), msg, function);
    MessageBox(g_hWnd, fullmsg, TEXT("Rad Switch"), MB_OK | MB_ICONERROR);
}

#define CHECK(x) if (!(x)) ShowLastError(TEXT(#x), TEXT(__FUNCTION__));

#if 0
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
#endif

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
                    //SendMessage(g_hWnd, WM_START, FALSE, (LPARAM) MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTOPRIMARY));
                    SendHotKey(HK_1_MOD, HK_1_KEY);
                    return 1;
                case VK_OEM_3: // Backtick
                    //PostMessage(g_hWnd, WM_START, TRUE, (LPARAM) MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTOPRIMARY));
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

RECT GetPosition(HMONITOR hMonitor)
{
    MONITORINFO MonitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &MonitorInfo);
    RECT r(MonitorInfo.rcMonitor);
    InflateRect(&r, Width(MonitorInfo.rcMonitor) / -4, Height(MonitorInfo.rcMonitor) / -3);
    return r;
}

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

    void FillList(HWND hActiveWnd, BOOL FilterToActive, HMONITOR hMonitor);
    void Switch(int iCaret);

    ListBoxOwnerDrawnFixed m_hWndChild;
    BOOL m_FilterToActive = FALSE;
    HMONITOR m_hMonitor = NULL;
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
        SetHandled(true);
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
        case VK_LEFT:
        {
            ShowWindow(*this, SW_HIDE);

            const std::vector<HMONITOR> ms = GetMonitors();
            const auto itOrig = std::find(ms.begin(), ms.end(), m_hMonitor);
            if (itOrig == ms.end())
                SendMessage(g_hWnd, WM_START, m_FilterToActive, (LPARAM) GetPrimaryMonitor(ms));
            else
            {
                auto it = itOrig;
                do
                {
                    it = it == ms.begin() ? ms.end() - 1 : std::prev(it);
                    if (SendMessage(g_hWnd, WM_START, m_FilterToActive, (LPARAM) *it))
                        break;
                } while (it != itOrig);
            }
            return -2;
        }
        case VK_RIGHT:
        {
            ShowWindow(*this, SW_HIDE);

            const std::vector<HMONITOR> ms = GetMonitors();
            const auto itOrig = std::find(ms.begin(), ms.end(), m_hMonitor);
            if (itOrig == ms.end())
                SendMessage(g_hWnd, WM_START, m_FilterToActive, (LPARAM) GetPrimaryMonitor(ms));
            else
            {
                auto it = itOrig;
                do
                {
                    it = it == (ms.end() - 1) ? ms.begin() : std::next(it);
                    if (SendMessage(g_hWnd, WM_START, m_FilterToActive, (LPARAM) *it))
                        break;
                } while (it != itOrig);
            }
            return -2;
        }
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
    case HK_1: if (GetForegroundWindow() != *this) PostMessage(g_hWnd, WM_START, FALSE, (LPARAM) MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTOPRIMARY)); break;
    case HK_2: if (GetForegroundWindow() != *this) PostMessage(g_hWnd, WM_START, TRUE, (LPARAM) MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTOPRIMARY)); break;
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
        SetHandled(TRUE);
        const HWND hActiveWnd = GetAncestor(GetForegroundWindow(), GA_ROOTOWNER);
        m_FilterToActive = (BOOL) wParam;
        m_hMonitor = (HMONITOR) lParam;

        FillList(hActiveWnd, m_FilterToActive, m_hMonitor);
        if (m_hWndChild.GetCount() > 0) // TODO Maybe > 1
        {
            if (true)
            {
                RECT rcWnd(GetPosition(m_hMonitor));
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
            ret = TRUE;
        }
    }
        break;
    case WM_STOP:
        Switch(m_hWndChild.GetCurSel());
        break;
    }

    if (!IsHandled())
        ret = m_hWndChild.HandleChainMessage(uMsg, wParam, lParam);
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

void RootWindow::FillList(HWND hActiveWnd, BOOL FilterToActive, HMONITOR hMonitor)
{
    TCHAR strActiveExe[MAX_PATH]{ TEXT("") };
    if (!FilterToActive || !QueryFullProcessImageNameFromHwnd(hActiveWnd, strActiveExe, ARRAYSIZE(strActiveExe)))
        strActiveExe[0] = TEXT('\0');


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
            if (IsWindowVisible(hWnd)
                && (CloakedVal == 0)
                && NoneSet(dwExStyle, WS_EX_TOOLWINDOW)
                && (GetWindow(hWnd, GW_OWNER) == NULL)
                && (hMonitor == NULL || MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY) == hMonitor))
            {
                TCHAR strExe[MAX_PATH] = TEXT("");
                QueryFullProcessImageNameFromHwnd(hWnd, strExe, ARRAYSIZE(strExe));

                if (FilterToActive && lstrcmp(strActiveExe, strExe) != 0)
                    continue;

                TCHAR title[1024];
                GetWindowText(hWnd, title, ARRAYSIZE(title));
                //if (IsMinimized(hWnd))
                    //lstrcat(title, TEXT("*"));

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
                m_hWndChild.SetItemGray(i, IsMinimized(hWnd));
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
        g_Theme.clrGrayText = RGB(128, 128, 128);
    }

    g_Theme.brWindow = CreateSolidBrush(g_Theme.clrWindow);
    g_Theme.brHighlight = CreateSolidBrush(g_Theme.clrHighlight);

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
