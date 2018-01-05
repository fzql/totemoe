// bili-settings.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-settings.hpp"
#include <codecvt>

#define SET_DEFAULT(section, key, value) \
    if (result != SI_FAIL) { result = SetDefault(section, key, value); }

SI_Error Bili::Settings::File::Load()
{
    SI_Error result;
    if (inifile.IsEmpty())
    {
        inifile.SetUnicode();
        result = inifile.LoadFile("config.ini");
        // [General]
        SET_DEFAULT(L"General", L"language", L"system");
        SET_DEFAULT(L"General", L"timeZone", L"system");
        // [Security]
        SET_DEFAULT(L"Security", L"liveCertificateChain", L"api.live.bilibili.com.crt");
        SET_DEFAULT(L"Security", L"enforceHttps", L"1");
        SET_DEFAULT(L"Security", L"enableMySQL", L"1");
        // [Session]
        SET_DEFAULT(L"Session", L"DedeUserID", L"");
        SET_DEFAULT(L"Session", L"DedeUserID__ckMd5", L"");
        SET_DEFAULT(L"Session", L"SESSDATA", L"");
        // [Danmaku]
        SET_DEFAULT(L"Danmaku", L"autoExport", L"csv");
        SET_DEFAULT(L"Danmaku", L"filterDanmaku", L"3");
        SET_DEFAULT(L"Danmaku", L"filterGifting", L"2");
        SET_DEFAULT(L"Danmaku", L"filterAnnouncement", L"2");
        SET_DEFAULT(L"Danmaku", L"filterUnknown", L"1");
        SET_DEFAULT(L"Danmaku", L"filterSmallTV", L"on");
        SET_DEFAULT(L"Danmaku", L"filterRegex", L"off");
        // [SendDanmaku]
        SET_DEFAULT(L"SendDanmaku", L"color", L"16777215");
        SET_DEFAULT(L"SendDanmaku", L"fontsize", L"25");
        SET_DEFAULT(L"SendDanmaku", L"mode", L"1");
        // [EndPoint]
        SET_DEFAULT(L"EndPoint", L"live", L"api.live.bilibili.com");
        // [LiveEndPoint]
        SET_DEFAULT(L"LiveEndPoint", L"player", L"/api/player");
        SET_DEFAULT(L"LiveEndPoint", L"roomInit", L"/room/v1/Room/room_init");
        SET_DEFAULT(L"LiveEndPoint", L"roomInfo", L"/room/v1/Room/get_info");
        SET_DEFAULT(L"LiveEndPoint", L"roomAnchor", L"/live_user/v1/UserInfo/get_anchor_in_room");
        SET_DEFAULT(L"LiveEndPoint", L"roomRecentChat", L"/ajax/msg");
        SET_DEFAULT(L"LiveEndPoint", L"userSendToRoom", L"/msg/send");
        SET_DEFAULT(L"LiveEndPoint", L"signInInfo", L"/sign/GetSignInfo");
        SET_DEFAULT(L"LiveEndPoint", L"danmakuSettings", L"/api/ajaxGetConfig");
        // [WebSocketEndPoint]
        SET_DEFAULT(L"WebSocketEndPoint", L"connect", L"/sub");
        // [RoomServer]
        SET_DEFAULT(L"RoomServer", L"heartBeatInterval", L"30000");
        SET_DEFAULT(L"RoomServer", L"pollingSleepTimer", L"50");
        // [MySQL]
        SET_DEFAULT(L"MySQL", L"host", L"localhost");
        SET_DEFAULT(L"MySQL", L"port", L"3306");
        SET_DEFAULT(L"MySQL", L"schema", L"maid_memory");
        SET_DEFAULT(L"MySQL", L"appUsername", L"TotemoeLiveMaid");
        SET_DEFAULT(L"MySQL", L"appPassword", L"i-love-bilibili-live");
    }
    else
    {
        result = SI_OK;
    }
    return result;
}

#undef SET_DEFAULT

SI_Error Bili::Settings::File::SetDefault(
    std::wstring const &section, std::wstring const &key, std::wstring const &value)
{
    SI_Error result;
    def.SetValue(section.c_str(), key.c_str(), value.c_str());
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

SI_Error Bili::Settings::File::Save()
{
    return inifile.SaveFile("config.ini");
}