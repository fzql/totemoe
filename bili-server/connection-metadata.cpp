
#include "stdafx.hpp"
#include "bili-server.hpp"
#include <fstream>
#include <future>
#include <thread>

Bili::Server::Connection::MetaData::MetaData(int id,
    websocketpp::connection_hdl hdl, std::string uri, std::string command) : 
    m_id(id), m_hdl(hdl), m_status("Connecting"), m_message_count(0),
    m_uri(uri), m_server("N/A"), m_command(command)
{
    m_messages.push_back(json({
        { "cmd", "WEBSOCKET" },
        { "msg", "Connecting" }
    }).dump());
}

void Bili::Server::Connection::MetaData::on_open(
    wsclient *c, websocketpp::connection_hdl hdl)
{
    m_status = "Open";

    wsclient::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");

    m_messages.push_back(json({
        { "cmd", "WEBSOCKET" },
        { "msg", "Established" }
    }).dump());

    std::string bytes = Bili::Server::Room::PacketHello(m_command);
    con->send(bytes, websocketpp::frame::opcode::binary);
}

void Bili::Server::Connection::MetaData::on_close(
    wsclient *c, websocketpp::connection_hdl hdl)
{
    m_status = "Closed";
    wsclient::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " ("
        << websocketpp::close::status::get_string(con->get_remote_close_code())
        << ")";
    m_error_reason = s.str();

    m_messages.push_back(json({
        { "cmd", "WEBSOCKET" },
        { "msg", "Closed" }
    }).dump());
}

void Bili::Server::Connection::MetaData::on_fail(
    wsclient *c, websocketpp::connection_hdl hdl)
{
    m_status = "Failed";

    wsclient::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();

    m_messages.push_back(json({
        { "cmd", "WEBSOCKET" },
        { "msg", "Failed" }
    }).dump());
}

void Bili::Server::Connection::MetaData::on_message(
    websocketpp::connection_hdl hdl, wsclient::message_ptr msg)
{
    // Ignore opcode::text.
    std::lock_guard<std::mutex> guard(m_mutex_messages);
    if (msg->get_opcode() == websocketpp::frame::opcode::binary)
    {
        // m_messages.push_back(msg->get_payload());
        auto bytes = msg->get_payload();
        auto length = bytes.size();

        if (length == 16)
        {
            // Server response, do nothing.
        }
        else if (length == 20)
        {
            // Audience count.
            m_audience = (bytes[0x10] << 24) | (bytes[0x11] << 16) |
                (bytes[0x12] << 8) | bytes[0x13];
        }
        else
        {
            size_t begin, end = 0;
            while (end < length)
            {
                begin = end;
                size_t size =
                    ((bytes[begin + 0] & 0xFFu) << 24) |
                    ((bytes[begin + 1] & 0xFFu) << 16) |
                    ((bytes[begin + 2] & 0xFFu) << 8) |
                    ((bytes[begin + 3] & 0xFFu));
                end = begin + size;
                begin += 16;
                std::string message(bytes.cbegin() + begin, bytes.cbegin() + end);
                m_messages.emplace_back(message);
                ++m_message_count;
            }
        }
    }
}

std::vector<std::string> Bili::Server::Connection::MetaData::retrieve()
{
    std::lock_guard<std::mutex> guard(m_mutex_messages);
    auto result = m_messages;
    m_messages.clear();
    return result;
}

std::ostream &Bili::Server::Connection::operator<< (std::ostream & ost,
    Bili::Server::Connection::MetaData const &data)
{
    ost << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason) << "\n"
        << "> Messages Received: (" << data.m_message_count << ")\n"
        << "> Messages Unprocessed: (" << data.m_messages.size() << ")\n";
    return ost;
}
