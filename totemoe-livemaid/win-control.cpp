
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
    default:
        return ::CallWindowProc(EditControl::defaultEditProc,
            hWnd, message, wParam, lParam);
    }
    return FALSE;
}