
// bili-server.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-server.hpp"
// #include "bili-settings/bili-settings.hpp"
#include "bili-https/bili-https.hpp"
#include <sstream>
#ifdef _DEBUG
#include <fstream>
#endif
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <algorithm>

std::string Bili::Server::Room::PacketHello(std::string const &command)
{
    std::string result;
    size_t bytes = 16 + command.length();
    result.resize(bytes);
    // Header: [Packet Length, 0x00100001, 0x00000007, 0x00000001]
    result[0x0] = (bytes >> 24) & 0xFF; 
    result[0x1] = (bytes >> 16) & 0xFF; 
    result[0x2] = (bytes >> 8) & 0xFF; 
    result[0x3] = bytes & 0xFF;
    result[0x4] = 0x00; result[0x5] = 0x10; result[0x6] = 0x00; result[0x7] = 0x01;
    result[0x8] = 0x00; result[0x9] = 0x00; result[0xA] = 0x00; result[0xB] = 0x07;
    result[0xC] = 0x00; result[0xD] = 0x00; result[0xE] = 0x00; result[0xF] = 0x01;
    std::copy(command.cbegin(), command.cend(), result.begin() + 16);
    return result;
}

std::string Bili::Server::Room::PacketHeart(std::string const &command)
{
    std::string result;
    size_t bytes = 16 + command.length();
    result.resize(bytes);
    // Header: [0x00000036, 0x00100001, 0x00000007, 0x00000001]
    result[0x0] = (bytes >> 24) & 0xFF;
    result[0x1] = (bytes >> 16) & 0xFF;
    result[0x2] = (bytes >> 8) & 0xFF;
    result[0x3] = bytes & 0xFF;
    result[0x4] = 0x00; result[0x5] = 0x10; result[0x6] = 0x00; result[0x7] = 0x01;
    result[0x8] = 0x00; result[0x9] = 0x00; result[0xA] = 0x00; result[0xB] = 0x02;
    result[0xC] = 0x00; result[0xD] = 0x00; result[0xE] = 0x00; result[0xF] = 0x01;
    std::copy(command.cbegin(), command.cend(), result.begin() + 16);
    return result;
}