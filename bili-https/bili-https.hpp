// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BILIGUEST_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BILIGUEST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#pragma once

#include "stdafx.hpp"
#include "bili-settings/bili-settings.hpp"
#include <algorithm>
#include <codecvt>
#include <ctime>
#include <locale>
#include <curl/curl.h>
#include <utf8/utf8/checked.h>

namespace Bili
{
    namespace Room
    {
        BILIHTTPS_API json GetDanmakuServer(ROOM roomid);

        BILIHTTPS_API json GetResolve(ROOM roomid);

        BILIHTTPS_API json GetInfo(ROOM roomid);

        BILIHTTPS_API json GetAnchorInfo(ROOM roomid);

        BILIHTTPS_API json GetRecentChat(ROOM roomid);

        BILIHTTPS_API json GetDanmakuSettings(ROOM roomid);

    }

    namespace User
    {
        struct Credentials final
        {
        public:
            // User ID
            std::string DedeUserID;
            // User Secret Hash
            std::string DedeUserID__ckMd5;
            // User Session
            std::string SESSDATA;
        public:
            // Convert to json
            BILIHTTPS_API friend void to_json(json &j, const Credentials &c)
            {
                j = json{
                    { "DedeUserID", c.DedeUserID },
                    { "DedeUserID__ckMd5", c.DedeUserID__ckMd5 },
                    { "SESSDATA", c.SESSDATA }
                };
            }
            // Convert from json
            BILIHTTPS_API friend void from_json(json const &j, Credentials &c)
            {
                c.DedeUserID = j.at("DedeUserID").get<std::string>();
                c.DedeUserID__ckMd5 = j.at("DedeUserID__ckMd5").get<std::string>();
                c.SESSDATA = j.at("SESSDATA").get<std::string>();
            }
        };

        struct SendOptions final
        {
        public:
            // Danmaku color
            int color;
            // Danmaku size
            int fontsize;
            // Danmaku mode
            int mode;
            // Random seed
            time_t rnd;
            // Room to send to
            int roomid;
            // Danmaku content
            std::wstring msg;
            // converter
        public:
            void prepare()
            {
                SendOptions backup = *this;
                Bili::Settings::File::Load();
                try
                {
                    color = std::stoi(Bili::Settings::Get(L"SendDanmaku", L"color"));
                    fontsize = std::stoi(Bili::Settings::Get(L"SendDanmaku", L"fontsize"));
                    mode = std::stoi(Bili::Settings::Get(L"SendDanmaku", L"mode"));
                    rnd = time(0);
                }
                catch (...)
                {
                    *this = backup;
                }
            }
            static std::string Encode(std::wstring const &wide)
            {
                std::string bytes;
                std::string result;
                utf8::utf32to8(wide.cbegin(), wide.cend(), std::back_inserter(bytes));
                curl_global_init(CURL_GLOBAL_DEFAULT);
                auto curl = curl_easy_init();
                char *encoded = curl_easy_escape(curl, bytes.c_str(), bytes.length());
                result.assign(encoded);
                curl_free(encoded);
                curl_easy_cleanup(curl);
                return result;
            }
            static std::wstring Decode(std::string const &escaped)
            {
                std::stoi("ABC");
                return std::wstring();
            }
        public:
            // Convert to json
            BILIHTTPS_API friend void to_json(json &j, const SendOptions &s)
            {
                j = json{
                    { "color", s.color },
                    { "fontsize", s.fontsize },
                    { "mode", s.mode },
                    { "rnd", s.rnd },
                    { "roomid", s.roomid },
                    { "msg", SendOptions::Encode(s.msg) }
                };
            }
            // Convert from json
            BILIHTTPS_API friend void from_json(json const &j, SendOptions &s)
            {
                s.color = j.at("color").get<size_t>();
                s.fontsize = j.at("fontsize").get<size_t>();
                s.mode = j.at("mode").get<size_t>();
                s.rnd = j.at("rnd").get<size_t>();
                s.roomid = j.at("roomid").get<size_t>();
                s.msg = SendOptions::Decode(j.at("msg").get<std::string>());
            }
        };

        BILIHTTPS_API json GetSignInInfo(Credentials const &cred);

        BILIHTTPS_API json SendRoomChat(Credentials const &cred,
            SendOptions const &options);
    }
}