#pragma once

#include <windowsx.h>

#undef HANDLE_MSG
#define HANDLE_MSG(message, fn)        case (message): SetHandled(true); ret = HANDLEX_##message((wParam), (lParam), (fn)); break

/* void Cls::OnCompacting(UINT compactRatio) */
#define HANDLEX_WM_COMPACTING(wParam, lParam, fn) \
    ((fn)((UINT)(wParam)), 0L)

/* void Cls::OnWinIniChange(LPCTSTR lpszSectionName) */
#define HANDLEX_WM_WININICHANGE(wParam, lParam, fn) \
    ((fn)((LPCTSTR)(lParam)), 0L)

/* void Cls::OnSysColorChange() */
#define HANDLEX_WM_SYSCOLORCHANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Cls::OnQueryNewPalette() */
#define HANDLEX_WM_QUERYNEWPALETTE(wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(), 0L)

/* void Cls::OnPaletteIsChanging(HWND hWndPaletteChange) */
#define HANDLEX_WM_PALETTEISCHANGING(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Cls::OnPaletteChanged(HWND hWndPaletteChange) */
#define HANDLEX_WM_PALETTECHANGED(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Cls::OnFontChange() */
#define HANDLEX_WM_FONTCHANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnSpoolerStatus(UINT status, int cJobInQueue) */
#define HANDLEX_WM_SPOOLERSTATUS(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Cls::OnDevModeChange(LPCTSTR lpszDeviceName) */
#define HANDLEX_WM_DEVMODECHANGE(wParam, lParam, fn) \
    ((fn)((LPCTSTR)(lParam)), 0L)

/* void Cls::OnTimeChange() */
#define HANDLEX_WM_TIMECHANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnPower(int code) */
#define HANDLEX_WM_POWER(wParam, lParam, fn) \
    ((fn)((int)(wParam)), 0L)

/* BOOL Cls::OnQueryEndSession() */
#define HANDLEX_WM_QUERYENDSESSION(wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(), 0L)

/* void Cls::OnEndSession(BOOL fEnding) */
#define HANDLEX_WM_ENDSESSION(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam)), 0L)

/* void Cls::OnQuit(int exitCode) */
#define HANDLEX_WM_QUIT(wParam, lParam, fn) \
    ((fn)((int)(wParam)), 0L)

/* This message is in Windows 3.1 only */
/* void Cls::OnSystemError(int errCode) */
#define HANDLEX_WM_SYSTEMERROR(wParam, lParam, fn) 0L

/* BOOL Cls::OnCreate(LPCREATESTRUCT lpCreateStruct) */
#define HANDLEX_WM_CREATE(wParam, lParam, fn) \
    ((fn)((LPCREATESTRUCT)(lParam)) ? 0L : (LRESULT)-1L)

/* BOOL Cls::OnNCCreate(LPCREATESTRUCT lpCreateStruct) */
#define HANDLEX_WM_NCCREATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((LPCREATESTRUCT)(lParam))

/* void Cls::OnDestroy() */
#define HANDLEX_WM_DESTROY(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnNCDestroy() */
#define HANDLEX_WM_NCDESTROY(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnShowWindow(BOOL fShow, UINT status) */
#define HANDLEX_WM_SHOWWINDOW(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam), (UINT)(lParam)), 0L)

/* void Cls::OnSetRedraw(BOOL fRedraw) */
#define HANDLEX_WM_SETREDRAW(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam)), 0L)

/* void Cls::OnEnable(BOOL fEnable) */
#define HANDLEX_WM_ENABLE(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam)), 0L)

/* void Cls::OnSetText(LPCTSTR lpszText) */
#define HANDLEX_WM_SETTEXT(wParam, lParam, fn) \
    ((fn)((LPCTSTR)(lParam)), 0L)

/* INT Cls::OnGetText(int cchTextMax, LPTSTR lpszText) */
#define HANDLEX_WM_GETTEXT(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((int)(wParam), (LPTSTR)(lParam))

/* INT Cls::OnGetTextLength() */
#define HANDLEX_WM_GETTEXTLENGTH(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)()

/* BOOL Cls::OnWindowPosChanging(LPWINDOWPOS lpwpos) */
#define HANDLEX_WM_WINDOWPOSCHANGING(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((LPWINDOWPOS)(lParam))

/* void Cls::OnWindowPosChanged(const LPWINDOWPOS lpwpos) */
#define HANDLEX_WM_WINDOWPOSCHANGED(wParam, lParam, fn) \
    ((fn)((const LPWINDOWPOS)(lParam)), 0L)

/* void Cls::OnMove(int x, int y) */
#define HANDLEX_WM_MOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Cls::OnSize(UINT state, int cx, int cy) */
#define HANDLEX_WM_SIZE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Cls::OnClose() */
#define HANDLEX_WM_CLOSE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Cls::OnQueryOpen() */
#define HANDLEX_WM_QUERYOPEN(wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(), 0L)

/* void Cls::OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo) */
#define HANDLEX_WM_GETMINMAXINFO(wParam, lParam, fn) \
    ((fn)((LPMINMAXINFO)(lParam)), 0L)

/* void Cls::OnPaint() */
#define HANDLEX_WM_PAINT(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Cls::OnEraseBkgnd(HDC hdc) */
#define HANDLEX_WM_ERASEBKGND(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((HDC)(wParam))

/* BOOL Cls::OnIconEraseBkgnd(HDC hdc) */
#define HANDLEX_WM_ICONERASEBKGND(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((HDC)(wParam))

/* void Cls::OnNCPaint(HRGN hrgn) */
#define HANDLEX_WM_NCPAINT(wParam, lParam, fn) \
    ((fn)((HRGN)(wParam)), 0L)

/* UINT Cls::OnNCCalcSize(BOOL fCalcValidRects, NCCALCSIZE_PARAMS * lpcsp) */
#define HANDLEX_WM_NCCALCSIZE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((BOOL)(wParam), (NCCALCSIZE_PARAMS *)(lParam))

/* UINT Cls::OnNCHitTest(int x, int y) */
#define HANDLEX_WM_NCHITTEST(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam))

/* HICON Cls::OnQueryDragIcon() */
#define HANDLEX_WM_QUERYDRAGICON(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)()

#ifdef _INC_SHELLAPI
/* void Cls::OnDropFiles(HDROP hdrop) */
#define HANDLEX_WM_DROPFILES(wParam, lParam, fn) \
    ((fn)((HDROP)(wParam)), 0L)
#endif  /* _INC_SHELLAPI */

/* void Cls::OnActivate(UINT state, HWND hWndActDeact, BOOL fMinimized) */
#define HANDLEX_WM_ACTIVATE(wParam, lParam, fn) \
    ((fn)((UINT)LOWORD(wParam), (HWND)(lParam), (BOOL)HIWORD(wParam)), 0L)

/* void Cls::OnActivateApp(BOOL fActivate, DWORD dwThreadId) */
#define HANDLEX_WM_ACTIVATEAPP(wParam, lParam, fn) \
    ((fn)((BOOL)(wParam), (DWORD)(lParam)), 0L)

/* BOOL Cls::OnNCActivate(BOOL fActive, HWND hWndActDeact, BOOL fMinimized) */
#define HANDLEX_WM_NCACTIVATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((BOOL)(wParam), 0L, 0L)

/* void Cls::OnSetFocus(HWND hWndOldFocus) */
#define HANDLEX_WM_SETFOCUS(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Cls::OnKillFocus(HWND hWndNewFocus) */
#define HANDLEX_WM_KILLFOCUS(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* void Cls::OnKey(UINT vk, int cRepeat, UINT flags) */
#define HANDLEX_WM_KEYDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Cls::OnKey(UINT vk, int cRepeat, UINT flags) */
#define HANDLEX_WM_KEYUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Cls::OnChar(TCHAR ch, int cRepeat) */
#define HANDLEX_WM_CHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Cls::OnDeadChar(TCHAR ch, int cRepeat) */
#define HANDLEX_WM_DEADCHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Cls::OnSysKey(UINT vk, int cRepeat, UINT flags) */
#define HANDLEX_WM_SYSKEYDOWN(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Cls::OnSysKey(UINT vk, int cRepeat, UINT flags) */
#define HANDLEX_WM_SYSKEYUP(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* void Cls::OnSysChar(TCHAR ch, int cRepeat) */
#define HANDLEX_WM_SYSCHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Cls::OnSysDeadChar(TCHAR ch, int cRepeat) */
#define HANDLEX_WM_SYSDEADCHAR(wParam, lParam, fn) \
    ((fn)((TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)

/* void Cls::OnMouseMove(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_MOUSEMOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnLButtonDown(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_LBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnLButtonDown(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_LBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnLButtonUp(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_LBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnRButtonDown(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_RBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnRButtonDown(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_RBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnRButtonUp(int x, int y, UINT flags) */
#define HANDLEX_WM_RBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnMButtonDown(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_MBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnMButtonDown(int x, int y, UINT keyFlags) */
#define HANDLEX_WM_MBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnMButtonUp(int x, int y, UINT flags) */
#define HANDLEX_WM_MBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnMouseWheel(int xPos, int yPos, int zDelta, UINT fwKeys) */
#define HANDLEX_WM_MOUSEWHEEL(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (int)(short)HIWORD(wParam), (UINT)(short)LOWORD(wParam)), 0L)

/* void Cls::OnNCMouseMove(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCMOUSEMOVE(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCLButtonDown(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCLBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCLButtonDown(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCLBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCLButtonUp(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCLBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCRButtonDown(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCRBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCRButtonDown(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCRBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCRButtonUp(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCRBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCMButtonDown(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCMBUTTONDOWN(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCMButtonDown(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCMBUTTONDBLCLK(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* void Cls::OnNCMButtonUp(int x, int y, UINT codeHitTest) */
#define HANDLEX_WM_NCMBUTTONUP(wParam, lParam, fn) \
    ((fn)((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)

/* int Cls::OnMouseActivate(HWND hWndTopLevel, UINT codeHitTest, UINT msg) */
#define HANDLEX_WM_MOUSEACTIVATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam))

/* void Cls::OnCancelMode() */
#define HANDLEX_WM_CANCELMODE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnTimer(UINT id) */
#define HANDLEX_WM_TIMER(wParam, lParam, fn) \
    ((fn)((UINT)(wParam)), 0L)

/* void Cls::OnInitMenu(HMENU hMenu) */
#define HANDLEX_WM_INITMENU(wParam, lParam, fn) \
    ((fn)((HMENU)(wParam)), 0L)

/* void Cls::OnInitMenuPopup(HMENU hMenu, UINT item, BOOL fSystemMenu) */
#define HANDLEX_WM_INITMENUPOPUP(wParam, lParam, fn) \
    ((fn)((HMENU)(wParam), (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam)), 0L)

/* void Cls::OnMenuSelect(HMENU hmenu, int item, HMENU hmenuPopup, UINT flags) */
#define HANDLEX_WM_MENUSELECT(wParam, lParam, fn)                  \
    ((fn)((HMENU)(lParam),  \
    (HIWORD(wParam) & MF_POPUP) ? 0L : (int)(LOWORD(wParam)),           \
    (HIWORD(wParam) & MF_POPUP) ? GetSubMenu((HMENU)lParam, LOWORD(wParam)) : 0L, \
    (UINT)(((short)HIWORD(wParam) == -1) ? 0xFFFFFFFF : HIWORD(wParam))), 0L)

/* DWORD Cls::OnMenuChar(UINT ch, UINT flags, HMENU hmenu) */
#define HANDLEX_WM_MENUCHAR(wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((UINT)(LOWORD(wParam)), (UINT)HIWORD(wParam), (HMENU)(lParam))

/* void Cls::OnCommand(int id, HWND hWndCtl, UINT codeNotify) */
#define HANDLEX_WM_COMMAND(wParam, lParam, fn) \
    ((fn)((int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)

/* LRESULT Cls::OnNotify(DWORD dwID, LPNMHDR pNmHdr) */
#define HANDLEX_WM_NOTIFY(wParam, lParam, fn) \
    (fn)((DWORD) wParam, (LPNMHDR) lParam)

/* void Cls::OnHScroll(HWND hWndCtl, UINT code, int pos) */
#define HANDLEX_WM_HSCROLL(wParam, lParam, fn) \
    ((fn)((HWND)(lParam), (UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam)), 0L)

/* void Cls::OnVScroll(HWND hWndCtl, UINT code, int pos) */
#define HANDLEX_WM_VSCROLL(wParam, lParam, fn) \
    ((fn)((HWND)(lParam), (UINT)(LOWORD(wParam)),  (int)(short)HIWORD(wParam)), 0L)

/* void Cls::OnCut() */
#define HANDLEX_WM_CUT(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnCopy() */
#define HANDLEX_WM_COPY(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnPaste() */
#define HANDLEX_WM_PASTE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnClear() */
#define HANDLEX_WM_CLEAR(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnUndo() */
#define HANDLEX_WM_UNDO(wParam, lParam, fn) \
    ((fn)(), 0L)

/* HANDLE Cls::OnRenderFormat(UINT fmt) */
#define HANDLEX_WM_RENDERFORMAT(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HANDLE)(fn)((UINT)(wParam))

/* void Cls::OnRenderAllFormats() */
#define HANDLEX_WM_RENDERALLFORMATS(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnClipboardUpdate() */
#define HANDLEX_WM_CLIPBOARDUPDATE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnDestroyClipboard() */
#define HANDLEX_WM_DESTROYCLIPBOARD(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnDrawClipboard() */
#define HANDLEX_WM_DRAWCLIPBOARD(wParam, lParam, fn) \
    ((fn)(), 0L)

/* void Cls::OnPaintClipboard(HWND hWndCBViewer, const LPPAINTSTRUCT lpPaintStruct) */
#define HANDLEX_WM_PAINTCLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (const LPPAINTSTRUCT)GlobalLock((HGLOBAL)(lParam))), GlobalUnlock((HGLOBAL)(lParam)), 0L)

/* void Cls::OnSizeClipboard(HWND hWndCBViewer, const LPRECT lprc) */
#define HANDLEX_WM_SIZECLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (const LPRECT)GlobalLock((HGLOBAL)(lParam))), GlobalUnlock((HGLOBAL)(lParam)), 0L)

/* void Cls::OnVScrollClipboard(HWND hWndCBViewer, UINT code, int pos) */
#define HANDLEX_WM_VSCROLLCLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (UINT)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Cls::OnHScrollClipboard(HWND hWndCBViewer, UINT code, int pos) */
#define HANDLEX_WM_HSCROLLCLIPBOARD(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (UINT)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* void Cls::OnAskCBFormatName(int cchMax, LPTSTR rgchName) */
#define HANDLEX_WM_ASKCBFORMATNAME(wParam, lParam, fn) \
    ((fn)((int)(wParam), (LPTSTR)(lParam)), 0L)

/* void Cls::OnChangeCBChain(HWND hWndRemove, HWND hWndNext) */
#define HANDLEX_WM_CHANGECBCHAIN(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (HWND)(lParam)), 0L)

/* BOOL Cls::OnSetCursor(HWND hWndCursor, UINT codeHitTest, UINT msg) */
#define HANDLEX_WM_SETCURSOR(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((HWND)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam))

/* void Cls::OnSysCommand(UINT cmd, int x, int y) */
#define HANDLEX_WM_SYSCOMMAND(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

/* HWND Cls::MDICreate(const LPMDICREATESTRUCT lpmcs) */
#define HANDLEX_WM_MDICREATE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((LPMDICREATESTRUCT)(lParam))

/* void Cls::MDIDestroy(HWND hWndDestroy) */
#define HANDLEX_WM_MDIDESTROY(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* NOTE: Usable only by MDI client windows */
/* void Cls::MDIActivate(BOOL fActive, HWND hWndActivate, HWND hWndDeactivate) */
#define HANDLEX_WM_MDIACTIVATE(wParam, lParam, fn) \
    ((fn)((BOOL)(lParam == (LPARAM)hWnd), (HWND)(lParam), (HWND)(wParam)), 0L)

/* void Cls::MDIRestore(HWND hWndRestore) */
#define HANDLEX_WM_MDIRESTORE(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* HWND Cls::MDINext(HWND hWndCur, BOOL fPrev) */
#define HANDLEX_WM_MDINEXT(wParam, lParam, fn) \
    (LRESULT)(HWND)(fn)((HWND)(wParam), (BOOL)lParam)

/* void Cls::MDIMaximize(HWND hWndMaximize) */
#define HANDLEX_WM_MDIMAXIMIZE(wParam, lParam, fn) \
    ((fn)((HWND)(wParam)), 0L)

/* BOOL Cls::MDITile(UINT cmd) */
#define HANDLEX_WM_MDITILE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((UINT)(wParam))

/* BOOL Cls::MDICascade(UINT cmd) */
#define HANDLEX_WM_MDICASCADE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((UINT)(wParam))

/* void Cls::MDIIconArrange() */
#define HANDLEX_WM_MDIICONARRANGE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* HWND Cls::MDIGetActive() */
#define HANDLEX_WM_MDIGETACTIVE(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(fn)()

/* HMENU Cls::MDISetMenu(BOOL fRefresh, HMENU hmenuFrame, HMENU hmenuWindow) */
#define HANDLEX_WM_MDISETMENU(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(fn)((BOOL)(wParam), (HMENU)(wParam), (HMENU)(lParam))

/* void Cls::OnChildActivate() */
#define HANDLEX_WM_CHILDACTIVATE(wParam, lParam, fn) \
    ((fn)(), 0L)

/* BOOL Cls::OnInitDialog(HWND hWndFocus, LPARAM lParam) */
#define HANDLEX_WM_INITDIALOG(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(BOOL)(fn)((HWND)(wParam), lParam)

/* HWND Cls::OnNextDlgCtl(HWND hWndSetFocus, BOOL fNext) */
#define HANDLEX_WM_NEXTDLGCTL(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HWND)(fn)((HWND)(wParam), (BOOL)(lParam))

/* void Cls::OnParentNotify(UINT msg, HWND hWndChild, int idChild) */
#define HANDLEX_WM_PARENTNOTIFY(wParam, lParam, fn) \
    ((fn)((UINT)LOWORD(wParam), (HWND)(lParam), (UINT)HIWORD(wParam)), 0L)

/* void Cls::OnEnterIdle(UINT source, HWND hWndSource) */
#define HANDLEX_WM_ENTERIDLE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (HWND)(lParam)), 0L)

/* UINT Cls::OnGetDlgCode(LPMSG lpmsg) */
#define HANDLEX_WM_GETDLGCODE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(UINT)(fn)((LPMSG)(lParam))

/* HBRUSH Cls::OnCtlColor(HDC hdc, HWND hWndChild, int type) */
#define HANDLEX_WM_CTLCOLORMSGBOX(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_MSGBOX)

#define HANDLEX_WM_CTLCOLOREDIT(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_EDIT)

#define HANDLEX_WM_CTLCOLORLISTBOX(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_LISTBOX)

#define HANDLEX_WM_CTLCOLORBTN(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_BTN)

#define HANDLEX_WM_CTLCOLORDLG(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_DLG)

#define HANDLEX_WM_CTLCOLORSCROLLBAR(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_SCROLLBAR)

#define HANDLEX_WM_CTLCOLORSTATIC(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HBRUSH)(fn)((HDC)(wParam), (HWND)(lParam), CTLCOLOR_STATIC)

/* void Cls::OnSetFont(HWND hWndCtl, HFONT hfont, BOOL fRedraw) */
#define HANDLEX_WM_SETFONT(wParam, lParam, fn) \
    ((fn)((HFONT)(wParam), (BOOL)(lParam)), 0L)

/* HFONT Cls::OnGetFont() */
#define HANDLEX_WM_GETFONT(wParam, lParam, fn) \
    (LRESULT)(UINT_PTR)(HFONT)(fn)()

/* void Cls::OnDrawItem(const DRAWITEMSTRUCT * lpDrawItem) */
#define HANDLEX_WM_DRAWITEM(wParam, lParam, fn) \
    ((fn)((const DRAWITEMSTRUCT *)(lParam)), 0L)

/* void Cls::OnMeasureItem(MEASUREITEMSTRUCT * lpMeasureItem) */
#define HANDLEX_WM_MEASUREITEM(wParam, lParam, fn) \
    ((fn)((MEASUREITEMSTRUCT *)(lParam)), 0L)

/* void Cls::OnDeleteItem(const DELETEITEMSTRUCT * lpDeleteItem) */
#define HANDLEX_WM_DELETEITEM(wParam, lParam, fn) \
    ((fn)((const DELETEITEMSTRUCT *)(lParam)), 0L)

/* int Cls::OnCompareItem(const COMPAREITEMSTRUCT * lpCompareItem) */
#define HANDLEX_WM_COMPAREITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((const COMPAREITEMSTRUCT *)(lParam))

/* int Cls::OnVkeyToItem(UINT vk, HWND hWndListbox, int iCaret) */
#define HANDLEX_WM_VKEYTOITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((UINT)LOWORD(wParam), (HWND)(lParam), (int)(short)HIWORD(wParam))

/* int Cls::OnCharToItem(UINT ch, HWND hWndListbox, int iCaret) */
#define HANDLEX_WM_CHARTOITEM(wParam, lParam, fn) \
    (LRESULT)(DWORD)(int)(fn)((UINT)LOWORD(wParam), (HWND)(lParam), (int)(short)HIWORD(wParam))

/* void Cls::OnQueueSync() */
#define HANDLEX_WM_QUEUESYNC(wParam, lParam, fn) \
    ((fn)(), 0L)

#if (WINVER >= 0x030a)
/* void Cls::OnCommNotify(int cid, UINT flags) */
#define HANDLEX_WM_COMMNOTIFY(wParam, lParam, fn) \
    ((fn)((int)(wParam), (UINT)LOWORD(lParam)), 0L)
#endif

/* void Cls::OnDisplayChange(UINT bitsPerPixel, UINT cxScreen, UINT cyScreen) */
#define HANDLEX_WM_DISPLAYCHANGE(wParam, lParam, fn) \
    ((fn)((UINT)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/* BOOL Cls::OnDeviceChange(UINT uEvent, DWORD dwEventData) */
#define HANDLEX_WM_DEVICECHANGE(wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((UINT)(wParam), (DWORD)(lParam))

/* void Cls::OnContextMenu(HWND hWndContext, UINT xPos, UINT yPos) */
#define HANDLEX_WM_CONTEXTMENU(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)), 0L)

#define HANDLEX_WM_COPYDATA(wParam, lParam, fn) \
    ((fn)((HWND)(wParam), (PCOPYDATASTRUCT)lParam), 0L)

/* void Cls::OnHotKey(int idHotKey, UINT fuModifiers, UINT vk) */
#define HANDLEX_WM_HOTKEY(wParam, lParam, fn) \
    ((fn)((int)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
