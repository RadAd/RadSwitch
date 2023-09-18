#include "Dialog.h"

extern HINSTANCE g_hInstance;
extern HWND g_hWndDlg;

namespace
{
    struct CreateDlgParams
    {
        LPARAM dwInitParam;
        bool bModal;
        Dialog* dlg;
    };
}

HWND Dialog::Create(Dialog* pDlg, int Resource, HWND hWndParent, LPARAM dwInitParam)
{
    CreateDlgParams p = { dwInitParam, false, pDlg };
    return CreateDialogParam(g_hInstance, MAKEINTRESOURCE(Resource), hWndParent, s_DlgProc, (LPARAM) &p);
}

INT_PTR Dialog::DoModal(Dialog* pDlg, int Resource, HWND hWndParent, LPARAM dwInitParam)
{
    CreateDlgParams p = { dwInitParam, true, pDlg };
    return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(Resource), hWndParent, s_DlgProc, (LPARAM) &p);
}

LRESULT Dialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

INT_PTR Dialog::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const HWND hWnd = *this;
    bool bHandled = false;
    const LRESULT ret = MessageHandler::ProcessMessage(uMsg, wParam, lParam, bHandled);

    //_ASSERTE(bHandled);
    SetWindowLongPtr(hWnd, DWLP_MSGRESULT, ret);

    switch (uMsg)
    {
    case WM_CHARTOITEM:
    case WM_COMPAREITEM:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    case WM_INITDIALOG:
    case WM_QUERYDRAGICON:
    case WM_VKEYTOITEM:
        return ret;

    default:
        return bHandled;
    }
}

INT_PTR CALLBACK Dialog::s_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Dialog* self = reinterpret_cast<Dialog*>(GetWindowLongPtr(hWnd, DWLP_USER));
    if (uMsg == WM_INITDIALOG)
    {
        CreateDlgParams* cdp = reinterpret_cast<CreateDlgParams*>(lParam);
        self = cdp->dlg;
        self->Set(hWnd);
        self->m_bModal = cdp->bModal;
        lParam = cdp->dwInitParam;
        SetWindowLongPtr(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(self));
    }
    else if (uMsg == WM_ACTIVATE)
    {
        if (LOWORD(wParam))
            g_hWndDlg = hWnd;
        else if (g_hWndDlg == hWnd)
            g_hWndDlg = NULL;
    }

    const INT_PTR ret = self ? self->ProcessMessage(uMsg, wParam, lParam) : 0;

    if (uMsg == WM_NCDESTROY)
    {
        _ASSERTE(self != nullptr);
        SetWindowLongPtr(hWnd, DWLP_USER, 0);
        self->Delete();
    }

    return ret;
}
