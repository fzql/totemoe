
#include "stdafx.hpp"
#include "bili-server.hpp"
#include <fstream>
#include <functional>   // std::bind

Bili::Server::Connection::EndPoint::~EndPoint()
{
    m_endpoint.stop_perpetual();

    for (auto it = m_connection_list.begin(); it != m_connection_list.end(); ++it)
    {
        if (it->second->getStatus() != "Open")
        {
            // Only close open connections
            continue;
        }
#ifdef _DEBUG
        std::ofstream file;
        file.open("frames.log", std::ofstream::out | std::ofstream::app);
        file << "> Closing connection " << it->second->getID() << std::endl;
        file.close();
#endif

        websocketpp::lib::error_code ec;
        m_endpoint.close(it->second->getHandle(), websocketpp::close::status::going_away, "", ec);
        if (ec)
        {
#ifdef _DEBUG
            std::ofstream file;
            file.open("frames.log", std::ofstream::out | std::ofstream::app);
            file << "> Error closing connection " << it->second->getID()
                << ": " << ec.message() << std::endl;
            file.close();
#endif
        }
    }

    m_thread->join();
}

int Bili::Server::Connection::EndPoint::connect(
    std::string const &uri, ROOM roomid)
{
    websocketpp::lib::error_code ec;

    wsclient::connection_ptr con = m_endpoint.get_connection(uri, ec);

    json connectJson = {
        { "uid", 0 },
        { "roomid", roomid },
        { "protover", 1 }
    };
    std::string command = connectJson.dump();

    if (ec)
    {
#ifdef _DEBUG
        std::ofstream file;
        file.open("frames.log", std::ofstream::out | std::ofstream::app);
        file << "> Connect initialization error: " << ec.message() << std::endl;
        file.close();
#endif
        return -1;
    }

    int new_id = m_next_id++;
    MetaData::ptr metadata_ptr(new MetaData(new_id, con->get_handle(), uri, command));
    m_connection_list[new_id] = metadata_ptr;

    con->set_open_handler(websocketpp::lib::bind(
        &MetaData::on_open,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_close_handler(websocketpp::lib::bind(
        &MetaData::on_close,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_fail_handler(websocketpp::lib::bind(
        &MetaData::on_fail,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_message_handler(websocketpp::lib::bind(
        &MetaData::on_message,
        metadata_ptr,
        websocketpp::lib::placeholders::_1,
        websocketpp::lib::placeholders::_2
    ));

    m_endpoint.connect(con);

    return new_id;
}

void Bili::Server::Connection::EndPoint::disconnect(
    int id, websocketpp::close::status::value code, std::string reason)
{
    websocketpp::lib::error_code ec;

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end())
    {
#ifdef _DEBUG
        std::ofstream file;
        file.open("frames.log", std::ofstream::out | std::ofstream::app);
        file << "> No connection found with id " << id << std::endl;
        file.close();
#endif
        return;
    }

    m_endpoint.close(metadata_it->second->getHandle(), code, "", ec);
    if (ec)
    {
#ifdef _DEBUG
        std::ofstream file;
        file.open("frames.log", std::ofstream::out | std::ofstream::app);
        file << "> Error initiating close: " << ec.message() << std::endl;
        file.close();
#endif
    }
}

void Bili::Server::Connection::EndPoint::send(int id, std::string message)
{
    websocketpp::lib::error_code ec;

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end())
    {
#ifdef _DEBUG
        std::ofstream file;
        file.open("frames.log", std::ofstream::out | std::ofstream::app);
        file << "> No connection found with id " << id << std::endl;
        file.close();
#endif
        return;
    }

    m_endpoint.send(metadata_it->second->getHandle(),
        message, websocketpp::frame::opcode::text, ec);
    if (ec)
    {
#ifdef _DEBUG
        std::ofstream file;
        file.open("frames.log", std::ofstream::out | std::ofstream::app);
        file << "> Error sending message: " << ec.message() << std::endl;
        file.close();
#endif
        return;
    }
}

std::vector<std::string> Bili::Server::Connection::EndPoint::retrieve(int id)
{
    std::vector<std::string> result;

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it != m_connection_list.end())
    {
        result = metadata_it->second->retrieve();
    }

    return result;
}

Bili::Server::Connection::MetaData::ptr
Bili::Server::Connection::EndPoint::getMetaData(int id) const
{
    con_list::const_iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end())
    {
        return MetaData::ptr();
    }
    else
    {
        return metadata_it->second;
    }
}