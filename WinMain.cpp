#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <exception>
#include <system_error>
//#include <objbase.h>
//#include <commctrl.h>

//#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hInstance = NULL;
HACCEL g_hAccelTable = NULL;
HWND g_hWndAccel = NULL;
HWND g_hWndDlg = NULL;

bool Run(_In_ const LPCTSTR lpCmdLine, _In_ const int nShowCmd);

int DoMessageLoop()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if ((g_hWndDlg == NULL || !IsDialogMessage(g_hWndDlg, &msg))
            && (g_hAccelTable == NULL || !TranslateAccelerator(g_hWndAccel, g_hAccelTable, &msg)))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return int(msg.wParam);
}

void DisplayError(const std::exception& e, const char* title)
{
    MessageBoxA(NULL, e.what(), title, MB_ICONERROR | MB_OK);
}

int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd)
try
{
    int ret = 0;
    g_hInstance = hInstance;
#ifdef _OBJBASE_H_  // from objbase.h
    if (SUCCEEDED(CoInitialize(nullptr)))
#endif
    {
#ifdef _INC_COMMCTRL    // from commctrl.h
        InitCommonControls();
#endif
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        if (Run(lpCmdLine, nShowCmd))
            ret = DoMessageLoop();
#ifdef _OBJBASE_H_
        CoUninitialize();
#endif
    }
    return ret;
}
catch (const std::system_error& e)
{
    char Filename[MAX_PATH];
    GetModuleFileNameA(NULL, Filename, ARRAYSIZE(Filename));
    DisplayError(e, Filename);
    return e.code().value();
}
catch (const std::exception& e)
{
    char Filename[MAX_PATH];
    GetModuleFileNameA(NULL, Filename, ARRAYSIZE(Filename));
    DisplayError(e, Filename);
    return 1;
}
