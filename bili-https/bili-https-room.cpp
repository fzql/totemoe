// bili-https-room.cpp : Defines the exported functions for the DLL application.

#include "stdafx.hpp"
#include "network.hpp"
#include "bili-https.hpp"
#include "bili-settings/bili-settings.hpp"
#include <regex>
// #include <libxml/HTMLparser.h>

#ifdef _MSC_VER
// Case-insensitive string comparison
#define COMPARE(a, b) (!_stricmp((a), (b)))
#else
#define COMPARE(a, b) (!strcasecmp((a), (b)))
#endif

// Source: https://stackoverflow.com/a/236803/1377770
template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

// Source: https://stackoverflow.com/a/236803/1377770
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

json Bili::Room::GetDanmakuServer(ROOM roomid)
{
    json result;

    auto url = Bili::Settings::GetAPI("live", "player");
    std::string response = curlHttpsGet(url, {
        { "id", "cid:" + std::to_string(roomid) },
        { "platform", "pc" },
        { "player_type", "web" }
    });
    std::regex rSimpleTag = std::regex("<(.*?)>(.*?)<\\/(?:.*?)>");
    std::sregex_iterator next(response.begin(), response.end(), rSimpleTag);
    std::sregex_iterator end;
    while (next != end) {
        std::smatch match = *next;
        std::string tag = match[1];
        std::string value = match[2];
        if (tag == "dm_server_list")
        {
            result[tag] = split(value, ',');
        }
        else
        {
            result[tag] = value;
        }
        next++;
    }
    return result;
}

json Bili::Room::GetResolve(ROOM roomid)
{
    auto url = Bili::Settings::GetAPI("live", "roomInit");
    return curlHttpsGet(url, roomid, {
        { "id", roomid }
    });
}

json Bili::Room::GetInfo(ROOM roomid)
{
    auto url = Bili::Settings::GetAPI("live", "roomInfo");
    return curlHttpsGet(url, roomid, {
        { "room_id", roomid }
    });
}

json Bili::Room::GetAnchorInfo(ROOM roomid)
{
    auto url = Bili::Settings::GetAPI("live", "roomAnchor");
    return curlHttpsGet(url, roomid, {
        { "roomid", roomid }
    });
}

json Bili::Room::GetRecentChat(ROOM roomid)
{
    auto url = Bili::Settings::GetAPI("live", "roomRecentChat");
    return curlHttpsPost(url, roomid, {
        { "post", { { "content", { { "roomid", roomid } } } } }
    });
}
