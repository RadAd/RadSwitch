#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <crtdbg.h>

class MessageHandler
{
public:
    HWND GetHWND() const { return m_hWnd; }
    operator HWND() const { return m_hWnd; }

protected:
    virtual ~MessageHandler() = default;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    INT_PTR ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    void SetHandled(bool bHandled) { m_msg->m_bHandled = bHandled; }
    bool IsHandled() const { return m_msg->m_bHandled; }
    void Set(HWND hWnd)
    {
        _ASSERTE(hWnd != NULL);
        _ASSERTE(m_hWnd == NULL);
        m_hWnd = hWnd;
    }
    void Delete() { m_delete = true; }

private:
    HWND m_hWnd = NULL;
    bool m_delete = false;

    struct Message
    {
        UINT        m_message;
        WPARAM      m_wParam;
        LPARAM      m_lParam;
        bool        m_bHandled;
    };

    Message* m_msg = nullptr;
};
