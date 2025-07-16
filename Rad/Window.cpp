#include "Window.h"
#include <set>
#include <algorithm>

extern HINSTANCE g_hInstance;

namespace
{
    struct CreateWndParams
    {
        LPVOID lpCreateParams;
        Window* wnd;
    };
}

HWND CreateWnd(const CREATESTRUCT& cs, const Window* wnd)
{
    CreateWndParams cwp = { cs.lpCreateParams, const_cast<Window*>(wnd) };
    return ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, &cwp);
}

inline void GetDefaultWndClass(WNDCLASS& wc)
{
    wc.style |= CS_DBLCLKS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
}

LPCTSTR MainClass::ClassName() { return TEXT("RadMain"); }

void MainClass::GetWndClass(WNDCLASS& wc)
{
    GetDefaultWndClass(wc);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
}

void MainClass::GetCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= WS_OVERLAPPEDWINDOW;
}

LPCTSTR ChildClass::ClassName() { return TEXT("RadChild"); }

void ChildClass::GetWndClass(WNDCLASS& wc)
{
    GetDefaultWndClass(wc);
}

void ChildClass::GetCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= WS_CHILD | WS_VISIBLE;
}

void Window::GetWndClass(WNDCLASS& wc)
{
    wc.lpfnWndProc = s_WndProc;
    wc.hInstance = g_hInstance;
}

LRESULT Window::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bHandled = false;
    const LRESULT ret = MessageHandler::ProcessMessage(uMsg, wParam, lParam, bHandled);
    _ASSERTE(bHandled);

    return ret;
}

LRESULT CALLBACK Window::s_WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    thread_local int calldepth = 0;
    thread_local std::set<Window*> todelete;
    ++calldepth;

    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        CreateWndParams* cwp = reinterpret_cast<CreateWndParams*>(lpcs->lpCreateParams);
        self = cwp->wnd;
        self->Set(hWnd);
        lpcs->lpCreateParams = cwp->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    else if (uMsg == WM_CREATE)
    {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        CreateWndParams* cwp = reinterpret_cast<CreateWndParams*>(lpcs->lpCreateParams);
        lpcs->lpCreateParams = cwp->lpCreateParams;
    }

    const LRESULT ret = self != nullptr
        ? self->ProcessMessage(uMsg, wParam, lParam)
        : DefWindowProc(hWnd, uMsg, wParam, lParam);

    if (self != nullptr && uMsg == WM_NCDESTROY)
        todelete.insert(self);

    --calldepth;
    if (calldepth == 0)
    {
        for (Window* wnd : todelete)
        {
            SetWindowLongPtr(*wnd, DWLP_USER, 0);
            delete wnd;
        }
    }

    return ret;
}

LRESULT Window::HandleMessage(const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    SetHandled(true);
    switch (uMsg)
    {
    case WM_PAINT:
        OnPaint();
        return 0;
    case WM_PRINTCLIENT:
        OnPrintClient(reinterpret_cast<HDC>(wParam));
        return 0;
    default:
        return DefWindowProc(*this, uMsg, wParam, lParam);
    }
}

void Window::OnPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(*this, &ps);
    OnDraw(&ps);
    EndPaint(*this, &ps);
}

void Window::OnPrintClient(const HDC hdc)
{
    PAINTSTRUCT ps = {};
    ps.hdc = hdc;
    GetClientRect(*this, &ps.rcPaint);
    OnDraw(&ps);
}
