
#include "stdafx.h"
#include "win-class.hpp"
#include "Resource.h"

WinBaseClass::WinBaseClass(int resourceId, HINSTANCE hInstance)
    : m_handle(hInstance)
{
    // WCHAR loadString[MAX_LOADSTRING];
    // TryLoadString(m_handle, resourceId, loadString, MAX_LOADSTRING);
    // m_name = loadString;
    m_name = ResourceString(hInstance, resourceId);
}

void WinBaseClass::UnRegister()
{
    UnregisterClass(m_name.c_str(), NULL);
}

HWND WinBaseClass::GetRunningWindow()
{
    HWND hWnd = ::FindWindow(GetName(), 0);
    if (::IsWindow(hWnd))
    {
        HWND hwndPopup = ::GetLastActivePopup(hWnd);
        if (::IsWindow(hwndPopup))
        {
            hWnd = hwndPopup;
        }
    }
    else
    {
        hWnd = nullptr;
    }
    return hWnd;
}