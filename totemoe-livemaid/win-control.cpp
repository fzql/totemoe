
#include "stdafx.h"
#include "win-control.hpp"
#include "bili-https/bili-https.hpp"

WNDPROC EditControl::defaultEditProc = nullptr;

LRESULT CALLBACK commandEditProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
        {
            ::SendMessage(::GetParent(hWnd), WM_COMMANDLINE_ENTER, NULL, NULL);
            return TRUE;
        }
        }

    case WM_CHAR:
        // To stop single-line edit from beeping upon return.
        // Reference: https://stackoverflow.com/a/3590126/1377770
        if (wParam == VK_RETURN)
        {
            return FALSE;
        }

    default:
        return ::CallWindowProc(EditControl::defaultEditProc,
            hWnd, message, wParam, lParam);
    }
    return FALSE;
}