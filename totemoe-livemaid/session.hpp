
#pragma once

#include "stdafx.h"
#include "win-control.hpp"

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

    ROOM getRoomID() const { return m_room.getRoomID(); }

    void reconnect();

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
private:

    bool m_bAutoReconnect;

    bool m_bStopThread;

    StatusBar *m_pStatusBar;

    TableListView *m_pTableListView;

    Bili::Server::Room m_room;

    std::thread m_thread;

    std::mutex m_mutex_message;

    std::vector<json> m_vMessage;

    std::vector<std::array<std::wstring, 9>> m_vDisplay;

    long m_nNextMessageID;
};
