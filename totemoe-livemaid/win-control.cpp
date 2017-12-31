
#include "stdafx.h"
#include "resource-i18n.hpp"
#include "win-control.hpp"
#include "bili-https/bili-https.hpp"

WNDPROC TableListView::defaultListViewProc = nullptr;

WNDPROC EditControl::defaultEditProc = nullptr;

LRESULT CALLBACK listViewProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CHAR:
    {
        if (wParam == 0x1)
        {
            if (GetKeyState(VK_CONTROL))
            {
                ::SendMessage(GetParent(hWnd), WM_COMMAND, IDM_DANMAKU_SELECTALL, NULL);
                return TRUE;
            }
        }
        else if (wParam == 0x3)
        {
            if (GetKeyState(VK_CONTROL))
            {
                int nSelected = ListView_GetSelectedCount(hWnd);
                if (nSelected > 0)
                {
                    ::SendMessage(GetParent(hWnd), WM_COMMAND, IDM_DANMAKU_COPYSELECTED, NULL);
                }
                return TRUE;
            }
        }
    }
    return TRUE;

    }
    return ::CallWindowProc(TableListView::defaultListViewProc,
        hWnd, message, wParam, lParam);
}

LRESULT CALLBACK commandEditProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_RETURN:
            // Catches return key.  Submit action.
            ::SendMessage(::GetParent(hWnd), WM_COMMANDLINE_ENTER, NULL, NULL);
            return TRUE;
        }
    }
    break;

    case WM_CHAR:
        if (wParam == VK_TAB)
        {
            if (GetKeyState(VK_SHIFT))
            {
                ::SendMessage(::GetParent(hWnd), WM_COMMANDTYPE_CYCLE, NULL, (LPARAM)-1);
            }
            else
            {
                ::SendMessage(::GetParent(hWnd), WM_COMMANDTYPE_CYCLE, NULL, (LPARAM)1);
            }
            return TRUE;
        }
        // Allow ESC key to clear all text in single-line edit.
        // Reference: https://stackoverflow.com/a/21358879/1377770
        else if (wParam == VK_ESCAPE)
        {
            int length = ::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
            if (length > 0)
            {
                ::SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)L"");
            }
            else
            {
                ::SendMessage(::GetParent(hWnd), WM_COMMANDTYPE_CYCLE, NULL, (LPARAM)0);
            }
            return TRUE;
        }
        // To stop single-line edit from beeping upon return.
        // Reference: https://stackoverflow.com/a/3590126/1377770
        else if (wParam == VK_RETURN)
        {
            return FALSE;
        }
    }
    return ::CallWindowProc(EditControl::defaultEditProc,
        hWnd, message, wParam, lParam);
}