
#pragma once

#include "stdafx.h"
#include "controller.hpp"
#include "bili-https/bili-https.hpp"

Controller::Controller(HWND hWnd, CREATESTRUCT *pCreate) :
    m_hWnd(hWnd), m_model(L"Generic"), m_statusBar(hWnd, IDC_STATUSBAR),
    m_listView(hWnd, IDC_LISTVIEW)
{
    m_statusBar.setParts({ 400, 600, 800 });
    // Initialize status bar.
    // Load these from string tables.
    m_statusBar.setText(0, L"Copyright (C) 2017 FrenzyLi");
    m_statusBar.setText(1, L"Cells");
    m_statusBar.setText(2, L"Text");
    m_statusBar.show();
    // Initialize header control.
    m_listView.setColumns({
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_ROOM),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_MSGID),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_TIME),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_PROTOCOL),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_USERID),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_USERTYPE),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_USERNAME),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_SUMMARY),
        (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_LIVEMAID_TRIGGER),
    }, { 80, 80, 150, 40, 80, 40, 180, 300, 40 });
    m_listView.show();
    // Initialize chat room.
    m_session.setTableListView(m_listView);
    m_session.setStatusBar(m_statusBar);
}

Controller::~Controller()
{
    try
    {
        m_thread_fetch.join();
    }
    catch (const std::system_error &)
    {
        /* suppressed */
    }
    ::PostQuitMessage(0);
}

void Controller::command(int cmd)
{
    switch (cmd)
    {
    case IDM_EXIT:
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0L);
        break;

    case IDM_HELP:
        ::MessageBox(m_hWnd, L"Go figure!", L"Generic",
            MB_ICONINFORMATION | MB_OK);
        break;

    case IDM_ABOUT:
        DialogBox(I18N::GetHandle(),
            MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, AboutDlgProc);
        break;

    case IDM_CONNECTTO:
        if (!IsWindow(m_hWndConnect))
        {
            m_hWndConnect = (HWND)CreateDialog(I18N::GetHandle(),
                MAKEINTRESOURCE(IDD_CONNECT), m_hWnd, ConnectDlgProc);
        }
        ShowWindow(m_hWndConnect, SW_SHOW);
        SetActiveWindow(m_hWndConnect);
        break;

    case IDM_DISCONNECT:
        m_session.disconnect();
        break;

    case IDM_RECONNECT:
        m_session.reconnect();
        break;

    case IDM_DANMAKU_COPYSELECTED:
        m_listView.setClipboard();
        break;

    case IDM_DANMAKU_SELECTALL:
        m_listView.selectAll();
        break;
    }
}

void Controller::connect()
{
    m_session.connect();
}

void Controller::disconnect()
{
    m_session.disconnect();
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
}

void Controller::initMenu(HMENU hMenu)
{
    int cMenuItems = GetMenuItemCount(hMenu);
    for (int nPos = 0; nPos < cMenuItems; nPos++)
    {
        UINT id = GetMenuItemID(hMenu, nPos);
        switch (id)
        {
        case IDM_CONNECTTO:
            if (m_session.isClosed())
            {
                EnableMenuItem(GetMenu(m_hWnd), id, MF_ENABLED);
            }
            else
            {
                EnableMenuItem(GetMenu(m_hWnd), id, MF_DISABLED | MF_GRAYED);
            }
            break;
        case IDM_RECONNECT:
        case IDM_DISCONNECT:
            if (m_session.isOpen())
            {
                EnableMenuItem(GetMenu(m_hWnd), id, MF_ENABLED);
            }
            else
            {
                EnableMenuItem(GetMenu(m_hWnd), id, MF_DISABLED | MF_GRAYED);
            }
            break;
        case IDM_DANMAKU_COPYSELECTED:
            int nSelected = m_listView.getSelectedCount();
            if (nSelected > 0)
            {
                EnableMenuItem(GetMenu(m_hWnd), id, MF_ENABLED);
            }
            break;
        }
    }
}

void Controller::paint()
{
    /*
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    // TODO: Add any drawing code that uses hdc here...
    EndPaint(hWnd, &ps);
    */
    PaintCanvas canvas(m_hWnd);
    m_view.Paint(canvas, m_model);
}

void Controller::setRoom(ROOM room)
{
    m_session.join(room);
    m_statusBar.setText(1, std::to_wstring(room));
}

void Controller::size(int cx, int cy)
{
    m_view.SetSize(cx, cy);
    m_statusBar.setSize(cx);

    // LONG height = m_statusBar.GetHeight();
    m_listView.setPosition(0, 0);
    m_listView.setSize(cx, cy - m_statusBar.getHeight());
}

// Message handler for about box.
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK ConnectDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_CONNECT_RESOLVE)
        {
            if (HIWORD(wParam) == EN_UPDATE)
            {
                WCHAR szContent[MAX_LOADSTRING];
                GetDlgItemText(hDlg, IDC_CONNECT_RESOLVE, szContent,
                    MAX_LOADSTRING);

                EnableWindow(GetDlgItem(hDlg, IDOK),
                    wcslen(szContent) > 0 ? TRUE : FALSE);

                return (INT_PTR)TRUE;
            }
        }
        else if (LOWORD(wParam) == IDOK)
        {
            WCHAR szContent[MAX_LOADSTRING];
            GetDlgItemText(hDlg, IDC_CONNECT_RESOLVE, szContent, MAX_LOADSTRING);
            std::wstring content = szContent;
            ROOM resolve = std::stoi(content);

            auto response = Bili::Room::GetResolve(resolve);

            // Ensure that the room being resolved is the current room.

            bool success = response.find("error") == response.end();
            if (success)
            {
                ROOM roomid = response["room_id"].get<ROOM>();
                SendMessage(GetParent(hDlg), WM_SET_ROOM, NULL, (LPARAM)roomid);
                SendMessage(GetParent(hDlg), WM_CONNECT, NULL, NULL);
                EndDialog(hDlg, LOWORD(wParam));
            }
            else
            {
                MessageBox(hDlg, L"LOL", L"NAY", MB_OK);
            }
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
