// bili-server.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-server.hpp"
#include "bili-https/bili-https.hpp"
#include <sstream>

std::string Bili::Server::GetURI(ROOM roomid)
{
    std::string result;
    json info = Bili::Room::GetDanmakuServer(roomid);
    if (!info.is_object() || info.find("dm_server_list") == info.end())
    {
        return "error";
    }
    // Fetch server list
    std::vector<std::string> servers = info["dm_server_list"];
    if (servers.size() <= 0)
    {
        return "error";
    }
    // Fetch top server.
    std::string address = servers.front();
    std::size_t portNum;
    // Fetch port number.
    std::string port;
    // Get secure websocket address
    std::stringstream sst;
    sst << "ws";
    if (false && strcmp(Bili::Settings::File::Get("Security", "enforceHttps"), "1") == 0)
    {
        port = info.at("dm_wss_port").get<std::string>();
        sst << "s";
    }
    else
    {
        port = info.at("dm_ws_port").get<std::string>();
    }
    try
    {
        portNum = std::stoi(port);
    }
    catch (...)
    {
        return "error";
    }
    Bili::Settings::File::Load();
    std::string resource = Bili::Settings::File::Get("WebSocketEndPoint", "connect");
    sst << "://" << address << ":" << portNum << resource;
    // Get URI
    result = sst.str();
    return result;
}
