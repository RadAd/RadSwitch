#include "AboutDlg.h"
#include "Rad/Windowxx.h"

#include <stdlib.h>
#include <Shellapi.h>

#include "resource.h"

extern HINSTANCE g_hInstance;

INT_PTR AboutDlg::DoModal(HWND hWndParent)
{
    return Dialog::DoModal(new AboutDlg, IDD_ABOUT, hWndParent);
}

BOOL AboutDlg::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
    SendMessage(GetDlgItem(*this, IDC_ABOUT_APPICON), STM_SETICON, (WPARAM) LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAIN)), 0);

    TCHAR	FileName[1024];
    GetModuleFileName(g_hInstance, FileName, 1024);

    DWORD	Dummy;
    DWORD	Size = GetFileVersionInfoSize(FileName, &Dummy);

    if (Size > 0)
    {
        void* Info = malloc(Size);
        if (Info != nullptr)
        {
            // VS_VERSION_INFO   VS_VERSIONINFO  VS_FIXEDFILEINFO

            //Dummy = 0;
            GetFileVersionInfo(FileName, Dummy, Size, Info);

            TCHAR* String;
            UINT	Length;
            VerQueryValue(Info, TEXT("\\StringFileInfo\\0c0904b0\\FileVersion"), (LPVOID*) &String, &Length);
            SetWindowText(GetDlgItem(*this, IDC_ABOUT_VERSION), String);
            VerQueryValue(Info, TEXT("\\StringFileInfo\\0c0904b0\\ProductName"), (LPVOID*) &String, &Length);
            SetWindowText(GetDlgItem(*this, IDC_ABOUT_PRODUCT), String);

            free(Info);
        }
    }
    else
    {
        SetWindowText(GetDlgItem(*this, IDC_ABOUT_VERSION), TEXT("Unknown"));
    }

    return TRUE;
}

void AboutDlg::OnCommand(int id, HWND hWndCtl, UINT codeNotify)
{
    if (codeNotify == BN_CLICKED)
    {
        switch (id)
        {
        case IDC_ABOUT_WEBSITE:
        case IDC_ABOUT_MAIL:
        {
            TCHAR	Url[1024];
            GetWindowText(hWndCtl, Url, 1024);
            ShellExecute(*this, TEXT("open"), Url, NULL, NULL, SW_SHOW);
        }
        break;
        case IDCANCEL:
        case IDOK:
        {
            EndDialog(*this, TRUE);
        }
        break;
        }
    }
}

LRESULT AboutDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;
    switch (uMsg)
    {
        HANDLE_MSG(WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(WM_COMMAND, OnCommand);
    }

    return IsHandled() ? ret : Dialog::HandleMessage(uMsg, wParam, lParam);
}
