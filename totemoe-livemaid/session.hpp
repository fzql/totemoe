
#pragma once

#include "stdafx.h"
#include "win-control.hpp"
#include "utf8/utf8.h"
#include <regex>
#include <fstream>

class MessageSession
{
public:

    MessageSession();

    ~MessageSession();
public:

    void join(ROOM room);

    bool isClosed() const;

    bool isOpen() const;

    void connect();

    void disconnect();

    LPCWSTR getColumn(int row, int col) const
    {
        LPCWSTR result;
        if (m_vFiltered.empty())
        {
            result = m_vDisplay[row][col].c_str();
        }
        else
        {
            result = m_vDisplay[m_vFiltered[row]][col].c_str();
        }
        return result;
    }

    ROOM getRoomID() const { return m_room.getRoomID(); }

    void reconnect();

    void setFilter(std::wstring const &keyword = std::wstring());

    void setFilterRegex(std::wstring const &regex);

    void setFilterCount();

    void setStatusBar(StatusBar &statusBar)
    {
        m_pStatusBar = &statusBar;
    }

    void setTableListView(TableListView &tableListView)
    {
        m_pTableListView = &tableListView;
    }

    void setAutoReconnect(bool option = true) { m_bAutoReconnect = option; };
private:

    void parseMessage(json const &object);

    void writeLine(std::wstring const &content);
private:

    bool m_bAutoReconnect;

    bool m_bStopThread;

    StatusBar *m_pStatusBar;

    TableListView *m_pTableListView;

    Bili::Server::Room m_room;
    // Used to filter displayed messages.
    std::wstring m_keyword;
    // Used to filter displayed messages.
    std::wregex m_regex;
    // Used to set filter type.
    bool m_bFilterRegex;

    std::tm m_tRecent;
    // History file.
    FILE *m_pFile;

    std::thread m_thread;

    std::mutex m_mutex_message;

    std::vector<json> m_vMessage;

    std::vector<std::array<std::wstring, 9>> m_vDisplay;

    std::vector<int> m_vFiltered;

    long m_nNextMessageID;

    bool m_bLockVScroll;
};
