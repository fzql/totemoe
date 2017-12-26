
#pragma once

#include "stdafx.h"
#include "win-class.hpp"

WinMaker::WinMaker(WinClass &winClass) :
    m_class(winClass), m_hWnd(nullptr), m_name(),
    m_dwStyle(WS_OVERLAPPEDWINDOW), m_x(CW_USEDEFAULT), m_y(CW_USEDEFAULT),
    m_nWidth(CW_USEDEFAULT), m_nHeight(CW_USEDEFAULT),
    m_hWndParent(nullptr), m_hMenu(nullptr), m_lpParam(nullptr)
{
}

void WinMaker::Create()
{
    m_hWnd = ::CreateWindowW(
        m_class.GetName(),      // name of a registered window class
        m_name.c_str(),         // window caption
        m_dwStyle,              // window style
        m_x,                    // x position
        m_y,                    // y position
        m_nWidth,               // width
        m_nHeight,              // height
        m_hWndParent,           // handle to parent window
        m_hMenu,                // handle to menu
        m_class.GetInstance(),  // application instance
        m_lpParam               // window creation data
    );

    if (m_hWnd == nullptr)
    {
        throw WinException(L"Internal error: Window Creation Failed.");
    }
}

void WinMaker::Show(int nCmdShow)
{
    ::ShowWindow(m_hWnd, nCmdShow);
    ::UpdateWindow(m_hWnd);
}
