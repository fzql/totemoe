
#pragma once

#include "stdafx.h"
#include "Canvas.hpp"
#include "bili-server/bili-server.hpp"
#include "win-control.hpp"
#include "session.hpp"

class PaintCanvas : public Canvas
{
public:
    // Constructor obtains the DC
    PaintCanvas(HWND hWnd)
        : Canvas(::BeginPaint(hWnd, &m_paint)), m_hWnd(hWnd)
    {}

    // Destructor releases the DC
    ~PaintCanvas()
    {
        ::EndPaint(m_hWnd, &m_paint);
    }
protected:

    PAINTSTRUCT m_paint;

    HWND m_hWnd;
};

class Model
{

    enum { TEXT_SIZE = 20 };
public:

    Model(LPCWSTR str)
    {
        SetText(str);
        m_text[TEXT_SIZE] = '\0';
    }

    void SetText(LPCWSTR str)
    {
        wcsncpy(m_text, str, TEXT_SIZE);
    }
public:

    LPCWSTR GetText() const { return m_text; }

    int GetLen() const { return wcslen(m_text); }

private:

    WCHAR m_text[TEXT_SIZE + 1];
};

class View
{
public:

    void SetSize(int cxNew, int cyNew)
    {
        m_cx = cxNew;
        m_cy = cyNew;
    }

    void Paint(Canvas & canvas, Model & model);
protected:

    int m_cx;

    int m_cy;
};

class Controller
{
public:

    Controller(HWND hWnd, CREATESTRUCT *pCreate);
    
    ~Controller();

    void command(int cmd);

    void connect();

    void disconnect();

    LPCWSTR getActiveSessionColumn(int row, int col);

    HWND getPropertySheet() const { return m_hPropSheet; }

    void initMenu(HMENU hMenu);

    void notify(LPNMHDR lpNMHdr);

    void cycleMode(LPARAM lParam);

    void paint();
    
    void setRoom(ROOM room);

    void showPropertySheet();
    
    void size(int x, int y);

    void submit();
private:

    static HMODULE  hI18N;

    HWND            m_hWnd;

    HWND            m_hWndConnect;

    Model           m_model;

    View            m_view;

    StatusBar       m_statusBar;

    TableListView   m_listView;

    ComboBox        m_commandType;

    EditControl     m_commandLine;

    MessageSession  m_session;

    HWND            m_hPropSheet;

    std::thread     m_thread_fetch;
};

INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK ConnectDlgProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK I18N_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK Danmaku_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK Session_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
