
#pragma once

#include "stdafx.h"
#include "win-class.hpp"
#include "win-exception.hpp"

void WinClass::Register()
{
    if (RegisterClassEx(&m_wClassEx) == 0)
    {
        throw WinException(L"Internal error: RegisterClassEx failed.");
    }
}

void WinClass::SetBgSysColor(INT sysColor)
{
    m_wClassEx.hbrBackground = (HBRUSH)(sysColor + 1);
}

void WinClass::SetIcon(INT resId)
{
    HANDLE handle = LoadIcon(GetInstance(), MAKEINTRESOURCE(resId));
    m_wClassEx.hIcon = (HICON)handle;
}

void WinClass::SetMenu(INT resId)
{
    HANDLE handle = MAKEINTRESOURCEW(resId);
    m_wClassEx.lpszMenuName = (LPCWSTR)handle;
}

void WinClass::SetSmallIcon(INT resId)
{
    HANDLE handle = LoadIcon(GetInstance(), MAKEINTRESOURCE(resId));
    m_wClassEx.hIconSm = (HICON)handle;
}


/*
WinClass::WinClass(WNDPROC winProc, LPCWSTR className, HINSTANCE hInstance)
{
    w_classex.cbSize = sizeof(WNDCLASSEX);

    w_classex.style = CS_HREDRAW | CS_VREDRAW;
    w_classex.lpfnWndProc = winProc;
    w_classex.cbClsExtra = 0;
    w_classex.cbWndExtra = 0;
    w_classex.hInstance = hInstance;
    w_classex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOTEMOECHAT));
    w_classex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    w_classex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    w_classex.lpszMenuName = MAKEINTRESOURCEW(IDC_TOTEMOECHAT);
    w_classex.lpszClassName = className;
    w_classex.hIconSm = LoadIcon(w_classex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
}
*/

void WinClass::SetDefaults()
{
    m_wClassEx.cbSize = sizeof(WNDCLASSEX);

    // Provide reasonable default values.
    m_wClassEx.style = CS_HREDRAW | CS_VREDRAW;
    m_wClassEx.cbClsExtra = 0;
    m_wClassEx.cbWndExtra = 0;
    m_wClassEx.hInstance = GetInstance();
    m_wClassEx.hIcon = nullptr;
    m_wClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_wClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    m_wClassEx.lpszMenuName = nullptr;
    m_wClassEx.lpszClassName = GetName();
    m_wClassEx.hIconSm = nullptr;
}
