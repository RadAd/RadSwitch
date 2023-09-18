#pragma once

#include "MessageHandler.h"

class Dialog : public MessageHandler
{
public:
    bool IsModal() const { return m_bModal; }

protected:
    static HWND Create(Dialog* pDlg, int Resource, HWND hWndParent, LPARAM dwInitParam = 0);
    static INT_PTR DoModal(Dialog* pDlg, int Resource, HWND hWndParent, LPARAM dwInitParam = 0);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    INT_PTR ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool m_bModal = false;
};
