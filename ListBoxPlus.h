#pragma once
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

struct Theme
{
    COLORREF clrWindow = GetSysColor(COLOR_WINDOW);
    COLORREF clrHighlight = GetSysColor(COLOR_HIGHLIGHT);
    COLORREF clrWindowText = GetSysColor(COLOR_WINDOWTEXT);
    COLORREF clrHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
    COLORREF clrGrayText = GetSysColor(COLOR_GRAYTEXT);
    HBRUSH   brWindow = NULL;
    HBRUSH   brHighlight = NULL;
};

extern Theme g_Theme;

class ListBox
{
public:
    void Create(_In_ HWND hParent, _In_ DWORD dwStyle, _In_ RECT rPos, _In_ int nID)
    {
        const HINSTANCE hInstance = NULL;
        m_hWnd = CreateWindow(WC_LISTBOX, nullptr, dwStyle, rPos.left, rPos.top, Width(rPos), Height(rPos), hParent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(nID)), hInstance, NULL);
    }

    operator bool() const { return m_hWnd != NULL; }

    operator HWND() const { return m_hWnd; }

    void ResetContent() { ListBox_ResetContent(m_hWnd); }
    int AddString(LPCTSTR lpsz) { return ListBox_AddString(m_hWnd, lpsz); }
    int GetCount() const { return ListBox_GetCount(m_hWnd); }
    int GetCurSel() const { return ListBox_GetCurSel(m_hWnd); }
    LPARAM GetItemData(int i) const { return ListBox_GetItemData(m_hWnd, i); }
    int GetItemRect(int i, LPRECT pRect) { return ListBox_GetItemRect(m_hWnd, i, pRect); }
    int GetText(int i, LPTSTR lpszBuffer) const { return ListBox_GetText(m_hWnd, i, lpszBuffer); }
    int SetCurSel(int i) { return ListBox_SetCurSel(m_hWnd, i); }
    int SetItemData(int i, LPARAM data) { return ListBox_SetItemData(m_hWnd, i, data); }
    int SetTopIndex(int i) { return ListBox_SetTopIndex(m_hWnd, i); }

private:
    HWND m_hWnd{ NULL };
};

class ListBoxOwnerDrawnFixed : public ListBox
{
private:
    struct ItemData
    {
        LPARAM lParam;
        HICON hIcon;
        TCHAR strRight[MAX_PATH];
        BOOL bGray;
    };

    ItemData* GetInternalItemData(int i)
    {
        const LPARAM lParam = ListBox::GetItemData(i);
        ItemData* pData = lParam == CB_ERR ? nullptr : reinterpret_cast<ItemData*>(lParam);
        if (pData == nullptr)
        {
            pData = new ItemData({});
            ListBox::SetItemData(i, reinterpret_cast<LPARAM>(pData));
        }
        return pData;
    }

    const ItemData* GetInternalItemData(int i) const
    {
        const LPARAM lParam = ListBox::GetItemData(i);
        ItemData* pData = lParam == CB_ERR ? nullptr : reinterpret_cast<ItemData*>(lParam);
        return pData;
    }

public:
    void Create(_In_ HWND hParent, _In_ DWORD dwStyle, _In_ RECT rPos, _In_ int nID)
    {
        m_nID = nID;
        ListBox::Create(hParent, dwStyle | LBS_OWNERDRAWFIXED, rPos, nID);
    }

    LPARAM GetItemData(int i) const
    {
        const ItemData* pData = GetInternalItemData(i);
        return pData ? pData->lParam : 0;
    }

    void SetItemData(int i, LPARAM data)
    {
        ItemData* pData = GetInternalItemData(i);
        pData->lParam = data;
    }

    void SetItemIcon(int i, HICON hIcon)
    {
        ItemData* pData = GetInternalItemData(i);
        pData->hIcon = hIcon;
        InvalidateItem(i);
    }

    void SetItemRightString(int i, LPCTSTR s)
    {
        ItemData* pData = GetInternalItemData(i);
        lstrcpy(pData->strRight, s);
        InvalidateItem(i);
    }

    void SetItemGray(int i, BOOL b)
    {
        ItemData* pData = GetInternalItemData(i);
        pData->bGray = b;
        InvalidateItem(i);
    }

    void InvalidateItem(int i)
    {
        if (IsWindow(*this))
        {
            RECT r;
            GetItemRect(i, &r);
            InvalidateRect(*this, &r, FALSE);
        }
    }

private:
    void OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItem)
    {
        //if (lpMeasureItem->CtlID == GetWindowLong(m_hWndChild, GWL_ID))
        if (lpMeasureItem->CtlID == m_nID)
        {
            lpMeasureItem->itemHeight = GetSystemMetrics(SM_CYICON) + 4;
        }
    }

    void OnDrawItem(const DRAWITEMSTRUCT* lpDrawItem)
    {
        if (lpDrawItem->hwndItem == *this)
        {
            if (lpDrawItem->itemID == -1) // Empty item
                return;

            const ItemData* pData = reinterpret_cast<ItemData*>(lpDrawItem->itemData);

            const COLORREF clrForeground = SetTextColor(lpDrawItem->hDC,
                lpDrawItem->itemState & ODS_SELECTED ? g_Theme.clrHighlightText : (pData->bGray ? g_Theme.clrGrayText : g_Theme.clrWindowText));

            const COLORREF clrBackground = SetBkColor(lpDrawItem->hDC,
                lpDrawItem->itemState & ODS_SELECTED ? g_Theme.clrHighlight : g_Theme.clrWindow);

            TCHAR text[1024];
            const int cch = GetText(lpDrawItem->itemID, text);

            FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, lpDrawItem->itemState & ODS_SELECTED ? g_Theme.brHighlight : g_Theme.brWindow);

            RECT rc = lpDrawItem->rcItem;
            rc.left = lpDrawItem->rcItem.right - Width(lpDrawItem->rcItem) / 4;
            DrawText(lpDrawItem->hDC, pData->strRight, lstrlen(pData->strRight), &rc, DT_VCENTER | DT_RIGHT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);

            rc.right = rc.left;
            rc.left = lpDrawItem->rcItem.left + GetSystemMetrics(SM_CXICON) + 4;
            DrawText(lpDrawItem->hDC, text, cch, &rc, DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX);

            SetTextColor(lpDrawItem->hDC, clrForeground);
            SetBkColor(lpDrawItem->hDC, clrBackground);

            DrawIcon(lpDrawItem->hDC, lpDrawItem->rcItem.left + 2, lpDrawItem->rcItem.top + 2, pData->hIcon);

            if (lpDrawItem->itemState & ODS_FOCUS)
                DrawFocusRect(lpDrawItem->hDC, &lpDrawItem->rcItem);
        }
    }

    void OnDeleteItem(const DELETEITEMSTRUCT* lpDeleteItem)
    {
        if (lpDeleteItem->hwndItem == *this)
        {
            ItemData* pData = reinterpret_cast<ItemData*>(lpDeleteItem->itemData);
            delete pData;
        }
    }

public:
    LRESULT HandleChainMessage(const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
    {
        LRESULT ret = 0;
        switch (uMsg)
        {
            HANDLE_MSG(WM_MEASUREITEM, OnMeasureItem);
            HANDLE_MSG(WM_DRAWITEM, OnDrawItem);
            HANDLE_MSG(WM_DELETEITEM, OnDeleteItem);
        }

        return ret;
    }

private:
    int m_nID{ 0 };
};
