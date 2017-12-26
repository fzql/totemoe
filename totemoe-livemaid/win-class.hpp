
#pragma once
#include "stdafx.h"

class WinBaseClass abstract
{
public:

    WinBaseClass(LPCWSTR className, HINSTANCE hInstance) :
        m_name(className), m_handle(hInstance)
    {}

    WinBaseClass(int resourceId, HINSTANCE hInstance);

    ~WinBaseClass()
    {
        UnRegister();
    }
public:

    LPCWSTR GetName() const
    {
        return m_name.c_str();
    }

    HINSTANCE GetInstance() const
    {
        return m_handle;
    }

    void UnRegister();

    HWND GetRunningWindow();
protected:

    HINSTANCE m_handle;

    std::wstring m_name;
};

class WinClass : public WinBaseClass
{
public:

    WinClass(WNDPROC winProc, LPCWSTR className, HINSTANCE hInstance) :
        WinBaseClass(className, hInstance)
    {
        m_wClassEx.lpfnWndProc = winProc;
        SetDefaults();
    }

    WinClass(WNDPROC winProc, int resClass, HINSTANCE hInstance) :
        WinBaseClass(resClass, hInstance)
    {
        m_wClassEx.lpfnWndProc = winProc;
        SetDefaults();
    }

public:

    void Register();

    void SetBgSysColor(INT sysColor);

    void SetIcon(INT resId);

    void SetMenu(INT resId);
    
    void SetSmallIcon(INT resId);
protected:

    void SetDefaults();

    WNDCLASSEX m_wClassEx;
};


class WinMaker
{
public:

    WinMaker(WinClass &winClass);

public:

    void Create();

    void Show(int cmdShow = SW_SHOWNORMAL);
public:

    void SetCaption(LPCWSTR caption)
    { 
        m_name = caption; 
    }

    void SetHScrollBar(BOOL option) 
    {
        if (option)
        {
            m_dwStyle |= WS_HSCROLL;
        }
        else
        {
            m_dwStyle &= ~WS_HSCROLL;
        }
    }

    void SetSysMenu(BOOL option)
    {
        if (option)
        {
            m_dwStyle |= WS_SYSMENU;
        }
        else
        {
            m_dwStyle &= ~WS_SYSMENU;
        }
    }

    void SetVScrollBar(BOOL option)
    {
        if (option)
        {
            m_dwStyle |= WS_VSCROLL;
        }
        else
        {
            m_dwStyle &= ~WS_VSCROLL;
        }
    }
public:

    operator HWND() { return m_hWnd; }

protected:

    BOOL initialized()
    {
        return m_hWnd != nullptr;
    }
protected:

    WinClass & m_class;

    HWND m_hWnd;

    std::wstring m_name;

    DWORD m_dwStyle;

    int m_x;

    int m_y;

    int m_nWidth;

    int m_nHeight;

    HWND m_hWndParent;

    HMENU m_hMenu;

    LPVOID m_lpParam;
};