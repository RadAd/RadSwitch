#include "MessageHandler.h"
#include <algorithm>

#if _UNICODE
#define _RPTFT0 _RPTFW0
#else
#define _RPTFT0 _RPTF0
#endif

LRESULT MessageHandler::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    Message m = { uMsg, wParam, lParam, false };
    Message* const pMsg = std::exchange(m_msg, &m);

    LRESULT ret = 0;
    try
    {
        ret = HandleMessage(uMsg, wParam, lParam);
    }
    catch (...)
    {
        _RPTFT0(_CRT_ERROR, TEXT("Unhandled exception"));
    }

    _ASSERTE(m_msg == &m);
    std::exchange(m_msg, pMsg);

    bHandled = m.m_bHandled;
    return ret;
}


LRESULT MessageChain::ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    Message m = { hWnd, uMsg, wParam, lParam, false };
    Message* const pMsg = std::exchange(m_msg, &m);

    LRESULT ret = 0;
    try
    {
        ret = HandleMessage(uMsg, wParam, lParam);
    }
    catch (...)
    {
        _RPTFT0(_CRT_ERROR, TEXT("Unhandled exception"));
    }

    _ASSERTE(m_msg == &m);
    std::exchange(m_msg, pMsg);

    bHandled = m.m_bHandled;
    return ret;
}
