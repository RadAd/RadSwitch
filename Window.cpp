#include "Window.h"
#include <cassert>
#include <algorithm>

extern HINSTANCE g_hInstance;

struct CreateWndParams
{
    LPVOID lpCreateParams;
    Window* wnd;
};

HWND CreateWnd(const CREATESTRUCT& cs, const Window* wnd)
{
    CreateWndParams cwp = { cs.lpCreateParams, const_cast<Window*>(wnd) };
    return ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, &cwp);
}

void Window::GetCreateWindow(CREATESTRUCT& cs)
{
    cs.x = CW_USEDEFAULT;
    cs.y = CW_USEDEFAULT;
    cs.cx = CW_USEDEFAULT;
    cs.cy = CW_USEDEFAULT;
}

void Window::GetWndClass(WNDCLASS& wc)
{
    wc.lpfnWndProc = s_WndProc;
    wc.hInstance = g_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
}

LRESULT Window::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Message m = { uMsg, wParam, lParam, false };
    Message* pMsg = &m;
    std::swap(pMsg, m_msg);

    const LRESULT ret = HandleMessage(uMsg, wParam, lParam);

    assert(IsHandled());
    assert(m_msg == &m);
    std::swap(pMsg, m_msg);

    return ret;
}

LRESULT CALLBACK Window::s_WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        CreateWndParams* cwp = reinterpret_cast<CreateWndParams*>(lpcs->lpCreateParams);
        self = cwp->wnd;
        self->m_hWnd = hWnd;
        lpcs->lpCreateParams = cwp->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    else if (uMsg == WM_CREATE)
    {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        CreateWndParams* cwp = reinterpret_cast<CreateWndParams*>(lpcs->lpCreateParams);
        lpcs->lpCreateParams = cwp->lpCreateParams;
    }

    LRESULT ret = self != nullptr
        ? self->ProcessMessage(uMsg, wParam, lParam)
        : DefWindowProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_NCDESTROY)
    {
        assert(self != nullptr);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
    }

    if (self != nullptr && GetWindowLongPtr(hWnd, GWLP_USERDATA) == 0 && self->m_msg == nullptr)
    {
        self->m_hWnd = NULL;
        delete self;
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
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
}

void Window::OnPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(m_hWnd, &ps);
    OnDraw(&ps);
    EndPaint(m_hWnd, &ps);
}

void Window::OnPrintClient(const HDC hdc)
{
    PAINTSTRUCT ps = {};
    ps.hdc = hdc;
    GetClientRect(m_hWnd, &ps.rcPaint);
    OnDraw(&ps);
}
