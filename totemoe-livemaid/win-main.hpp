
#pragma once

#include "stdafx.h"
#include "win-class.hpp"

class WinMainClass : public WinClass
{
public:

    WinMainClass(WNDPROC wndProc, int resClass, HINSTANCE hInstance) :
        WinClass(wndProc, resClass, hInstance)
    {
        SetIcon(IDI_TOTEMOELIVEMAID);
        SetSmallIcon(IDI_SMALL);
        // SetMenu(IDC_TOTEMOE_LIVEMAID);
    }
};

class WinMainMaker : public WinMaker
{
public:

    WinMainMaker(WinClass &winClass, LPCWSTR lpTitle) :
        WinMaker(winClass)
    {
        m_hMenu = LoadMenu(I18N::GetHandle(), MAKEINTRESOURCE(IDC_TOTEMOELIVEMAID));
        ::SetMenu(m_hWnd, m_hMenu);
        SetCaption(lpTitle);
    }

    WinMainMaker(WinClass &winClass, int resTitle) :
        WinMaker(winClass)
    {
        // WCHAR loadString[MAX_LOADSTRING];
        // TryLoadString(winClass.GetInstance(), resTitle, loadString, MAX_LOADSTRING);
        // m_name = loadString;
        m_hMenu = LoadMenu(I18N::GetHandle(), MAKEINTRESOURCE(IDC_TOTEMOELIVEMAID));
        ::SetMenu(m_hWnd, m_hMenu);
        m_name = ResourceString(winClass.GetInstance(), resTitle);
    }
};