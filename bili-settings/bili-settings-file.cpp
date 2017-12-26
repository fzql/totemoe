// bili-settings.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-settings.hpp"

#define SET_DEFAULT(section, key, value) \
    if (result != SI_FAIL) { result = SetDefault(section, key, value); }

SI_Error Bili::Settings::File::Load()
{
    SI_Error result;
    if (ini.IsEmpty())
    {
        ini.SetUnicode();
        result = ini.LoadFile("config.ini");
        // [Security]
        SET_DEFAULT("General", "language", "en-US");
        // [Security]
        SET_DEFAULT("Security", "liveCertificateChain", "api.live.bilibili.com.crt");
        SET_DEFAULT("Security", "enforceHttps", "1");
        SET_DEFAULT("Security", "enableMySQL", "1");
        // [Credentials]
        SET_DEFAULT("Credentials", "DedeUserID", "");
        SET_DEFAULT("Credentials", "DedeUserID__ckMd5", "");
        SET_DEFAULT("Credentials", "SESSDATA", "");
        // [SendDanmaku]
        SET_DEFAULT("SendDanmaku", "color", "16777215");
        SET_DEFAULT("SendDanmaku", "fontsize", "25");
        SET_DEFAULT("SendDanmaku", "mode", "1");
        // [EndPoint]
        SET_DEFAULT("EndPoint", "live", "api.live.bilibili.com");
        // [LiveEndPoint]
        SET_DEFAULT("LiveEndPoint", "player", "/api/player");
        SET_DEFAULT("LiveEndPoint", "roomInit", "/room/v1/Room/room_init");
        SET_DEFAULT("LiveEndPoint", "roomInfo", "/room/v1/Room/get_info");
        SET_DEFAULT("LiveEndPoint", "roomAnchor", "/live_user/v1/UserInfo/get_anchor_in_room");
        SET_DEFAULT("LiveEndPoint", "roomRecentChat", "/ajax/msg");
        SET_DEFAULT("LiveEndPoint", "userSendToRoom", "/msg/send");
        // [WebSocketEndPoint]
        SET_DEFAULT("WebSocketEndPoint", "connect", "/sub");
        // [RoomServer]
        SET_DEFAULT("RoomServer", "heartBeatInterval", "30000");
        SET_DEFAULT("RoomServer", "pollingSleepTimer", "50");
        // [MySQL]
        SET_DEFAULT("MySQL", "host", "localhost");
        SET_DEFAULT("MySQL", "port", "3306");
        SET_DEFAULT("MySQL", "schema", "maid_memory");
        SET_DEFAULT("MySQL", "appUsername", "TotemoeLiveMaid");
        SET_DEFAULT("MySQL", "appPassword", "i-love-bilibili-live");
    }
    else
    {
        result = SI_OK;
    }
    return result;
}

#undef SET_DEFAULT

SI_Error Bili::Settings::File::SetDefault(
    std::string const &section, std::string const &key, std::string const &value)
{
    SI_Error result;
    if (Get(section, key) == nullptr)
    {
        result = Set(section, key, value.c_str());
    }
    else
    {
        result = SI_OK;
    }
    return result;
}

char const *Bili::Settings::File::Get(
    std::string const &section, std::string const &key, char const *value)
{
    return ini.GetValue(section.c_str(), key.c_str(), value);
}

SI_Error Bili::Settings::File::Set(
    std::string const &section, std::string const &key, char const *value)
{
    return ini.SetValue(section.c_str(), key.c_str(), value);
}

SI_Error Bili::Settings::File::Save()
{
    return ini.SaveFile("config.ini");
}