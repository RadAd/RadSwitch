#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "NewDebug.h"

class MessageHandler
{
public:
    HWND GetHWND() const { return m_hWnd; }
    operator HWND() const { return m_hWnd; }

protected:
    virtual ~MessageHandler() = default;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    void SetHandled(bool bHandled) { m_msg->m_bHandled = bHandled; }
    bool IsHandled() const { return m_msg->m_bHandled; }
    void Set(HWND hWnd)
    {
        _ASSERTE(hWnd != NULL);
        _ASSERTE(m_hWnd == NULL);
        m_hWnd = hWnd;
    }

private:
    HWND m_hWnd = NULL;

    struct Message
    {
        UINT        m_message;
        WPARAM      m_wParam;
        LPARAM      m_lParam;
        bool        m_bHandled;
    };

    Message* m_msg = nullptr;
};

class MessageChain
{
public:
    LRESULT ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
    virtual ~MessageChain() = default;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    void SetHandled(bool bHandled) { m_msg->m_bHandled = bHandled; }
    bool IsHandled() const { return m_msg->m_bHandled; }

    HWND GetChainWnd() const { return m_msg->m_hWnd;  }

private:
    struct Message
    {
        HWND        m_hWnd;
        UINT        m_message;
        WPARAM      m_wParam;
        LPARAM      m_lParam;
        bool        m_bHandled;
    };

    Message* m_msg = nullptr;
};
