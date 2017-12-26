
#pragma once

#include "stdafx.h"

class Canvas abstract
{
public:

    void Line(int x1, int y1, int x2, int y2)
    {
        ::MoveToEx(m_hdc, x1, y1, 0);
        ::LineTo(m_hdc, x2, y2);
    }

    void Text(int x, int y, LPCWSTR buf, int cBuf)
    {
        ::TextOut(m_hdc, x, y, buf, cBuf);
    }

    void Char(int x, int y, WCHAR c)
    {
        ::TextOut(m_hdc, x, y, &c, 1);
    }

public:

    operator HDC ()
    {
        return m_hdc;
    }
protected:
    //  Protected constructor: You can't construct a Canvas object, but you may
    //  be able to construct objects derived from it.
    Canvas(HDC hdc) : m_hdc(hdc) {}
protected:
    //  Handle to device context.
    HDC m_hdc;
};
