#pragma once

#include "MessageHandler.h"

class Window;

template<class T>
inline ATOM Register()
{
    WNDCLASS wc = {};
    Window::GetWndClass(wc);
    wc.lpszClassName = T::ClassName();
    T::GetWndClass(wc);
    _ASSERT(wc.lpfnWndProc != nullptr);
    _ASSERT(wc.hInstance != NULL);
    _ASSERT(wc.lpszClassName != NULL);
    return ::RegisterClass(&wc);
}

HWND CreateWnd(const CREATESTRUCT& cs, const class Window* wnd);

template<class T>
class WindowManager
{
public:
    static void GetCreateWindow(CREATESTRUCT& cs)
    {
        cs.lpszClass = T::Class::ClassName();
        T::Class::GetCreateWindow(cs);
    }

    static T* Create(HWND hWndParent = NULL, LPCTSTR pTitle = nullptr, LPVOID lpCreateParams = nullptr)
    {
        CREATESTRUCT cs = {};
        cs.hwndParent = hWndParent;
        cs.lpszName = pTitle;
        cs.lpCreateParams = lpCreateParams;
        cs.x = CW_USEDEFAULT;
        cs.y = CW_USEDEFAULT;
        cs.cx = CW_USEDEFAULT;
        cs.cy = CW_USEDEFAULT;
        GetCreateWindow(cs);
        return Create(cs);
    }

    static T* Create(CREATESTRUCT& cs)
    {
        T* self = new DEBUG_NEW T();
        self->GetCreateWindow(cs);
        if (self && CreateWnd(cs, self) != NULL)
            return self;
        else
        {
            delete self;
            return nullptr;
        }
    }
};

struct MainClass
{
    static LPCTSTR ClassName();
    static void GetWndClass(WNDCLASS& wc);
    static void GetCreateWindow(CREATESTRUCT& cs);
};

struct ChildClass
{
    static LPCTSTR ClassName();
    static void GetWndClass(WNDCLASS& wc);
    static void GetCreateWindow(CREATESTRUCT& cs);
};

class Window : public MessageHandler
{
public:
    static void GetWndClass(WNDCLASS& cs);

protected:
    virtual void GetCreateWindow(CREATESTRUCT& cs) {};
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual void OnDraw(const PAINTSTRUCT* pps) const {}

private:
    void OnPaint();
    void OnPrintClient(HDC hdc);

    LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
