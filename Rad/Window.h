#pragma once

#include "MessageHandler.h"

HWND CreateWnd(const CREATESTRUCT& cs, const class Window* wnd);

template<class T>
class WindowManager
{
public:
    static void GetWndClass(WNDCLASS& cs)
    {
        cs.lpszClassName = T::ClassName();
        T::GetWndClass(cs);
    }

    static ATOM Register()
    {
        WNDCLASS wc = {};
        GetWndClass(wc);
        return ::RegisterClass(&wc);
    }

    static void GetCreateWindow(CREATESTRUCT& cs)
    {
        cs.lpszClass = T::ClassName();
        T::GetCreateWindow(cs);
    }

    static T* Create(LPVOID lpCreateParams = nullptr)
    {
        CREATESTRUCT cs = {};
        cs.lpCreateParams = lpCreateParams;
        GetCreateWindow(cs);
        return Create(cs);
    }

    static T* Create(const CREATESTRUCT& cs)
    {
        T* self = new T();
        if (self && CreateWnd(cs, self) != NULL)
            return self;
        else
        {
            delete self;
            return nullptr;
        }
    }
};

class Window : public MessageHandler
{
protected:
    static void GetCreateWindow(CREATESTRUCT& cs);
    static void GetWndClass(WNDCLASS& wc);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual void OnDraw(const PAINTSTRUCT* pps) const { }

private:
    void OnPaint();
    void OnPrintClient(HDC hdc);

    LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
