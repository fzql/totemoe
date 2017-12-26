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
    if (m_roomid == 0)
    {
        return;
    }
    // Resolve uri
    std::string uri = Bili::Server::GetURI(m_roomid);
    m_id = m_endpoint.connect(uri, m_roomid);
    if (m_id != -1)
    {
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
                std::this_thread::sleep_for(1s);
            }
        });
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

/*
void Bili::Server::sendHeartBeatToRoom(Room &room)
{
    using namespace std::chrono_literals;
    size_t quit = false;
    size_t ticks = 0;
    while (!quit)
    {
        auto metadata = room.getMetaData();
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
        if (ticks % 6 == 0 && status == "Open")
        {
            room.send(Bili::Server::Room::PacketHeart("[object Object]"));
        }
        std::this_thread::sleep_for(5s);
        if (status == "Open")
        {
            ticks = (ticks + 1) % 6;
        }
    }
}

void Bili::Server::Room::on_message(wsclient* c, websocketpp::connection_hdl hdl,
    websocketpp::config::asio_client::message_type::ptr msg)
{
#ifdef _DEBUG
    std::ofstream file;
    file.open("frames.log", std::ofstream::out | std::ofstream::app);
    file << "on_message called with hdl: " << hdl.lock().get()
        << " and message: " << msg->get_payload()
        << std::endl;
    file.close();
#endif
    websocketpp::lib::error_code ec;

    // c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    // if (ec) {
    //    std::cout << "Echo failed because: " << ec.message() << std::endl;
    // }
}
*/

