#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
//#include <objbase.h>
//#include <commctrl.h>

HINSTANCE g_hInstance = NULL;
HACCEL g_hAccelTable = NULL;

bool Run(_In_ const LPCTSTR lpCmdLine, _In_ const int nShowCmd);

int DoMessageLoop()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        // TODO IsDialogMessage()
        if (g_hAccelTable == NULL || !TranslateAccelerator(msg.hwnd, g_hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return int(msg.wParam);
}

int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd)
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
