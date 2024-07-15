#pragma once
#include "Dialog.h"

class AboutDlg : public Dialog
{
public:
    static INT_PTR DoModal(HWND hWndParent);

private:
    BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
    void OnCommand(int id, HWND hWndCtl, UINT codeNotify);

protected:
    using Dialog::DoModal;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};
