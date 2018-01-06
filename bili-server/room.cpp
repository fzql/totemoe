// bili-server.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-server.hpp"
// #include "bili-settings/bili-settings.hpp"
#include "bili-https/bili-https.hpp"
#include <sstream>
// #include <mutex>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void Bili::Server::Room::join(ROOM room)
{
    m_roomid = room;
}

void Bili::Server::Room::send(std::string const &msg)
{
    m_endpoint.send(m_id, msg);
}

void Bili::Server::Room::connect()
{
    if (m_roomid == 0 && !m_heart_beat.joinable())
    {
        return;
    }
    // Resolve uri
    std::string uri = Bili::Server::GetURI(m_roomid);
    m_id = m_endpoint.connect(uri, m_roomid);
    if (m_id != -1)
    {
        // ============================================ DETATCH A THREAD ==== //
        // ==== To send heartbeats to the server at a regular interval ====== //
        m_heart_beat = std::thread([this]()
        {
            bool quit = false;
            size_t ticks = 0;
            size_t every_tick = 30;
            while (!quit)
            {
                auto metadata = getMetaData();
                if (metadata == nullptr)
                {
                    quit = true;
                    continue;
                }
                auto status = metadata->getStatus();
                if (status == "Closed")
                {
                    quit = true;
                    continue;
                }
                if (ticks % every_tick == 0 && status == "Open")
                {
                    send(Bili::Server::Room::PacketHeart("[object Object]"));
                }
                using namespace std::chrono_literals;
                if (status == "Open")
                {
                    ticks = (ticks + 1) % every_tick;
                }
                if (!quit)
                {
                    std::this_thread::sleep_for(1s);
                }
            }
        });
        m_heart_beat.detach();
    }
}

void Bili::Server::Room::disconnect(websocketpp::close::status::value code, std::string const &reason)
{
    m_endpoint.disconnect(m_id, code, reason);
    try
    {
        m_heart_beat.join();
    }
    catch (...)
    {

    }
}

void Bili::Server::Room::heartBeat()
{
    send(Bili::Server::Room::PacketHeart("[object Object]"));
}

std::vector<std::string> Bili::Server::Room::retrieve()
{
    return m_endpoint.retrieve(m_id);
}

void Bili::Server::Room::clear()
{
    m_roomid = 0;
}
