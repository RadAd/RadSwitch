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

#if _DEBUG
void __cdecl CrtDebugEnd()
{
    _CrtMemState state;
    _CrtMemCheckpoint(&state);

    if (state.lCounts[_CLIENT_BLOCK] != 0 ||
        state.lCounts[_NORMAL_BLOCK] != 0 ||
        (_crtDbgFlag & _CRTDBG_CHECK_CRT_DF && state.lCounts[_CRT_BLOCK] != 0))
    {
        _RPT0(_CRT_ERROR, "Detected memory leaks!\n");
    }
}

void CrtDebugInitFile(LPCTSTR lpDebugFileName)
{
    const HANDLE hLogFile = CreateFile(lpDebugFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    _ASSERTE(hLogFile);
    if (hLogFile && hLogFile != INVALID_HANDLE_VALUE)
    {
        for (int e = _CRT_WARN; e < _CRT_ERRCNT; ++e)
        {
            _CrtSetReportMode(e, _CrtSetReportMode(e, _CRTDBG_REPORT_MODE) | _CRTDBG_MODE_FILE);
            _CrtSetReportFile(e, hLogFile);
        }
    }
}

void CrtDebugInit()
{
    TCHAR Filename[MAX_PATH];
    const DWORD len = GetModuleFileName(NULL, Filename, ARRAYSIZE(Filename));
    _ASSERTE(_tcscmp(Filename + len - 4, TEXT(".exe")) == 0);
    _tcscpy_s(Filename + len - 4, ARRAYSIZE(Filename) - len + 4, TEXT("DbgLog.txt"));
    CrtDebugInitFile(Filename);
    atexit(CrtDebugEnd);
}
#else
#define CrtDebugInit()                   ((int)0)
#endif

int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd)
try
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    CrtDebugInit();

    int ret = EXIT_FAILURE;
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
    return EXIT_FAILURE;
}
