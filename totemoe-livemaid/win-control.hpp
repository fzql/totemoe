
#pragma once

#include "stdafx.h"
#include "Resource.h"
#include <vector>
#include <CommCtrl.h>

class WinControl abstract
{
public:

    WinControl(HWND hWndParent, int id)
        : m_hWnd(::GetDlgItem(hWndParent, id))
    {}

    void disable()
    {
        ::EnableWindow(m_hWnd, FALSE);
    }

    void enable()
    {
        ::EnableWindow(m_hWnd, TRUE);
    }

    HWND getHandle() const
    {
        return m_hWnd;
    }

    virtual LONG getHeight()
    {
        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        return rect.bottom - rect.top;
    }

    virtual LONG getWidth()
    {
        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        return rect.right - rect.left;
    }

    void hide()
    {
        ::ShowWindow(m_hWnd, SW_HIDE);
    }

    BOOL isVisible()
    {
        return(::IsWindowVisible(m_hWnd));
    }

    void show()
    {
        ::ShowWindow(m_hWnd, SW_SHOW);
    }

    void setFocus()
    {
        ::SetFocus(m_hWnd);
    }

    virtual void setPosition(int X, int Y)
    {
        SetWindowPos(m_hWnd, NULL, X, Y, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER);
        ::SendMessage(m_hWnd, WM_SIZE, 0, 0);
    }

    virtual void setSize(int cx, int cy)
    {
        SetWindowPos(m_hWnd, NULL, NULL, NULL, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
        ::SendMessage(m_hWnd, WM_SIZE, 0, 0);
    }

protected:

    HWND m_hWnd;

    LONG m_nHeight;

    LONG m_nWidth;
};

class StatusBar : public WinControl
{
public:

    StatusBar(HWND hWndParent, int id) :
        WinControl(hWndParent, id)
    {
        m_hWnd = ::CreateWindowEx(
            NULL,
            STATUSCLASSNAME,
            nullptr,
            WS_CHILD | WS_VISIBLE | SBT_TOOLTIPS | WS_EX_WINDOWEDGE, // | SBARS_SIZEGRIP,
            0, 0, 0, 0,
            hWndParent,
            (HMENU)id,
            WinGetLong<HINSTANCE>(hWndParent, GWL_HINSTANCE),
            nullptr
        );

        if (!m_hWnd)
        {
            throw WinException(L"Failed To Create The Status Bar");
        }
    }
public:

    LONG getHeight() const { return m_nHeight; }

    void setParts(std::vector<int> const &rights)
    {
        m_vInitRight = rights;
        size_t parts = m_vInitRight.size();

        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        LONG widthBar = rect.right - rect.left;
        LONG widthTotal = m_vInitRight.back();

        m_vRight.resize(parts);
        for (size_t i = 0; i + 1 < parts; ++i)
        {
            m_vRight[i] = m_vInitRight[i] - (widthTotal - widthBar);
        }
        m_vRight.back() = -1;

        if (m_vText.size() < parts)
        {
            m_vText.resize(parts);
        }
        ::SendMessage(m_hWnd, SB_SETPARTS, parts, (LPARAM)m_vRight.data());
    }

    void setText(int part, std::wstring const &text)
    {
        m_vText[part] = text;
        ::SendMessage(m_hWnd, SB_SETTEXT, part, (LPARAM)m_vText[part].data());
    }

    void setSize(int width)
    {
        size_t parts = m_vInitRight.size();

        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        m_nHeight = rect.bottom - rect.top;
        m_nWidth = rect.right - rect.left;
        LONG widthTotal = m_vInitRight.back();

        m_vRight.resize(m_vInitRight.size());
        for (size_t i = 0; i + 1 < parts; ++i)
        {
            m_vRight[i] = m_vInitRight[i] - (widthTotal - width);
        }
        m_vRight.back() = -1;
        ::SendMessage(m_hWnd, SB_SETPARTS, parts, (LPARAM)m_vRight.data());
        ::SendMessage(m_hWnd, WM_SIZE, 0, 0);
    }
protected:

    HWND m_hWnd;

    std::vector<int> m_vInitRight;

    std::vector<int> m_vRight;

    std::vector<std::wstring> m_vText;
};

class TableListView : public WinControl
{
public:

    TableListView(HWND hWndParent, int id) :
        WinControl(hWndParent, id), m_bLockVScroll(false)
    {
        m_hWnd = ::CreateWindow(WC_LISTVIEW,
            L"",
            WS_CHILD | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hWndParent,
            (HMENU)id,
            WinGetLong<HINSTANCE>(hWndParent, GWL_HINSTANCE),
            NULL
        );

        if (!m_hWnd)
        {
            throw WinException(L"Failed To Create The List View");
        }

        ListView_SetExtendedListViewStyleEx(m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    }
public:

    HWND getHeader()
    {
        return ListView_GetHeader(m_hWnd);
    }

    int getSelectedCount()
    {
        return ListView_GetSelectedCount(m_hWnd);
    }

    void notify(LPARAM lParam)
    {

    }

    void push_back(std::array<std::wstring, 9> const &pushes)
    {
        LVITEM lvItem;

        // Initialize LVITEM members that are common to all items.
        lvItem.pszText = const_cast<LPWSTR>(pushes[0].c_str());
        lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
        lvItem.stateMask = 0;
        lvItem.iItem = ::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0);
        lvItem.iSubItem = 0;
        lvItem.state = 0;

        // Insert one column.
        SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

        // Initialize LVITEM members that are different for each item.
        size_t cItems = pushes.size();
        for (size_t index = 1; index < cItems && index < m_vColumns.size(); index++)
        {
            lvItem.iSubItem = index;
            lvItem.pszText = const_cast<LPWSTR>(pushes[index].c_str());
            SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvItem);
        }

        if (!m_bLockVScroll)
        {
            scrollToBottom();
        }
    }

    void scrollToBottom()
    {
        int nItems = ::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0);
        SendMessage(m_hWnd, LVM_ENSUREVISIBLE, nItems - 1, TRUE);
    }

    void selectAll()
    {
        int nItems = ::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0);
        for (int iItem = 0; iItem < nItems; ++iItem)
        {
            if (iItem + 1 < nItems)
            {
                ListView_SetItemState(m_hWnd, iItem, LVIS_SELECTED,
                    LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT);
            }
            else
            {
                ListView_SetItemState(m_hWnd, iItem, LVIS_SELECTED | LVIS_FOCUSED,
                    LVIS_FOCUSED | LVIS_SELECTED | LVIS_CUT);
            }
        }
    }

    void setClipboard()
    {
        // Get the first selected item
        LVITEM lvItem;
        WCHAR szText[MAX_LOADSTRING];
        lvItem.cchTextMax = MAX_LOADSTRING;
        lvItem.pszText = szText;
        // Get the index of the first selected item.
        int iPos = ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
        std::wstringstream wss;
        if (iPos != -1)
        {
            for (size_t iCol = 0; iCol < m_vColumns.size(); ++iCol)
            {
                wss << m_vColumns[iCol];
                if (iCol + 1 < m_vColumns.size())
                {
                    wss << L"\t";
                }
                else
                {
                    wss << L"\r\n";
                }
            }
        }
        while (iPos != -1)
        {
            for (size_t iCol = 0; iCol < m_vColumns.size(); ++iCol)
            {
                lvItem.iSubItem = iCol;
                ::SendMessage(m_hWnd, LVM_GETITEMTEXT, iPos, (LPARAM)&lvItem);
                if (wcslen(szText) >> 0)
                {
                    wss << szText;
                }
                else
                {
                    wss << L" ";
                }
                if (iCol + 1 < m_vColumns.size())
                {
                    wss << L"\t";
                }
                else
                {
                    wss << L"\r\n";
                }
            }
            // Get the next selected itemB
            iPos = ListView_GetNextItem(m_hWnd, iPos, LVNI_SELECTED);
        }
        std::wstring text = wss.str();
        bool success = false;
        if (OpenClipboard(m_hWnd))
        {
            const size_t size = (text.length() + 1) * sizeof(WCHAR);
            HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, size);
            if (hClipboardData)
            {
                LPWSTR pchData = (LPWSTR)GlobalLock(hClipboardData);
                if (pchData)
                {
                    wcscpy(pchData, text.data());
                    EmptyClipboard();
                    GlobalUnlock(hClipboardData);
                    SetClipboardData(CF_UNICODETEXT, hClipboardData);
                }
            }
        }
        CloseClipboard();
    }

    void setColumns(std::vector<std::wstring> const &names,
        std::vector<int> const &widths)
    {
        m_vColumns = names;

        LVCOLUMN lvc;

        // Initialize the LVCOLUMN structure.
        // The mask specifies that the format, width, text,
        // and subitem members of the structure are valid.
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

        // Add the columns.
        size_t nCols = names.size();
        for (size_t iCol = 0; iCol < nCols; iCol++)
        {
            lvc.iSubItem = iCol;
            lvc.pszText = const_cast<LPWSTR>(m_vColumns[iCol].data());
            // Width of column in pixels.
            lvc.cx = widths[iCol];

            if (iCol + 1 < nCols)
            {
                lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
            }
            else
            {
                lvc.fmt = LVCFMT_LEFT; // Right-aligned column.
            }
            // Insert the columns into the list view.
            if (ListView_InsertColumn(m_hWnd, (int)iCol, &lvc) == -1)
            {
                throw WinException(L"Failed to insert column into List View");
            }
        }
    }

protected:

    std::vector<std::wstring> m_vColumns;

    bool m_bLockVScroll;
};

class ComboBox : public WinControl
{
public:

    ComboBox(HWND hWndParent, int id) :
        WinControl(hWndParent, id)
    {
        m_hWnd = ::CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,
            WC_COMBOBOX,
            NULL,
            CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_SIMPLE | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hWndParent,
            (HMENU)id,
            WinGetLong<HINSTANCE>(hWndParent, GWL_HINSTANCE),
            NULL
        );

        // Reference: https://stackoverflow.com/a/6057761/1377770
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        HFONT hDlgFont = CreateFontIndirect(&(ncm.lfMessageFont));
        ::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hDlgFont, TRUE);
    }
public:

    void add(std::wstring const &content)
    {
        ::SendMessage(m_hWnd, CB_ADDSTRING, (WPARAM)NULL, (LPARAM)content.c_str());
    }

    void setSelection(int index)
    {
        ::SendMessage(m_hWnd, CB_SETCURSEL, (WPARAM)index, NULL);
    }

    void setSelectionNext()
    {
        int nCount = ::SendMessage(m_hWnd, CB_GETCOUNT, NULL, NULL);
        if (nCount > 0)
        {
            int iItem = ::SendMessage(m_hWnd, CB_GETCURSEL, NULL, NULL);
            iItem = (iItem + 1) % nCount;
            ::SendMessage(m_hWnd, CB_SETCURSEL, (WPARAM)iItem, NULL);
        }
    }

    void setSelectionPrev()
    {
        int nCount = ::SendMessage(m_hWnd, CB_GETCOUNT, NULL, NULL);
        if (nCount > 0)
        {
            int iItem = ::SendMessage(m_hWnd, CB_GETCURSEL, NULL, NULL);
            iItem = (iItem + (nCount - 1)) % nCount;
            ::SendMessage(m_hWnd, CB_SETCURSEL, (WPARAM)iItem, NULL);
        }
    }

    std::wstring getText()
    {
        WCHAR szContent[MAX_LOADSTRING];
        ::GetWindowText(m_hWnd, szContent, MAX_LOADSTRING);
        return std::wstring(szContent);
    }
};

LRESULT CALLBACK commandEditProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class EditControl : public WinControl
{
public:

    EditControl(HWND hWndParent, int id) :
        WinControl(hWndParent, id)
    {
        m_hWnd = ::CreateWindow(
            WC_EDIT,
            NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hWndParent,
            (HMENU)id,
            WinGetLong<HINSTANCE>(hWndParent, GWL_HINSTANCE),
            NULL
        );

        // Reference: https://stackoverflow.com/a/6057761/1377770
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        HFONT hDlgFont = CreateFontIndirect(&(ncm.lfMessageFont));
        ::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hDlgFont, TRUE);

        defaultEditProc = (WNDPROC)
            ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)commandEditProc);
    }

    std::wstring getContent()
    {
        WCHAR szContent[MAX_LOADSTRING];
        GetWindowText(m_hWnd, szContent, MAX_LOADSTRING);
        return std::wstring(szContent);
    }

    void clear()
    {
        ::SendMessage(m_hWnd, WM_SETTEXT, NULL, (LPARAM)L"");
    }

public:

    static WNDPROC defaultEditProc;
};