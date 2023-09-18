#include "MessageHandler.h"
#include <algorithm>

LRESULT MessageHandler::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    Message m = { uMsg, wParam, lParam, false };
    Message* const pMsg = std::exchange(m_msg, &m);

    const LRESULT ret = HandleMessage(uMsg, wParam, lParam);

    _ASSERTE(m_msg == &m);
    std::exchange(m_msg, pMsg);

    if (m_delete && m_msg == nullptr)
        delete this;

    bHandled = m.m_bHandled;
    return ret;
}
