#include "stdafx.h"
#include "CppUnitTest.h"
#include "bili-https/bili-https.hpp"
#include "bili-settings/bili-settings.hpp"
#include <curl/curl.h>

#ifdef _DEBUG
#include <locale>
#include <codecvt>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace totemoeunittest
{
    TEST_CLASS(BiliHttps)
    {
    public:

        TEST_METHOD(JSONParsing)
        {
            json a = "{\"code\":0,\"msg\":\"success\",\"message\":\"success\",\"data\":{\"info\":{\"uid\":168598,\"uname\":\"xxxx\",\"face\":\"http://i2.hdslb.com/bfs/face/d0dad8800774a4903547b1326d1fd927df47b4e9.jpg\",\"rank\":\"10000\",\"platform_user_level\":6,\"mobile_verify\":0,\"official_verify\":{\"type\":-1,\"desc\":\"\"},\"vip_type\":0},\"level\":{\"uid\":\"168598\",\"cost\":\"636400\",\"rcost\":\"12288437666\",\"user_score\":\"53892500\",\"vip\":0,\"vip_time\":\"0000-00-00 00:00:00\",\"svip\":\"0\",\"svip_time\":\"0000-00-00 00:00:00\",\"update_time\":\"2017-12-20 12:55:38\",\"master_level\":{\"level\":39,\"current\":[22500000,122013810],\"next\":[25000000,147013810],\"anchor_score\":122884376,\"upgrade_score\":24129434,\"sort\":6},\"user_level\":25,\"color\":5805790,\"anchor_score\":122884376},\"san\":12}}"_json;
            json b;
            b = json::parse("{\"code\":0,\"msg\":\"success\",\"message\":\"success\",\"data\":{\"info\":{\"uid\":4278633,\"uname\":\"FrenzyLi\",\"face\":\"https://i2.hdslb.com/bfs/face/69460509aaeb887867771b60b81fcc7c115e285a.jpg\",\"rank\":10000,\"identification\":1,\"mobile_verify\":1,\"platform_user_level\":5,\"official_verify\":{\"type\":-1,\"desc\":\"\"}},\"level\":{\"uid\":\"4278633\",\"cost\":\"29684600\",\"rcost\":\"59836874\",\"user_score\":514189750,\"vip\":0,\"vip_time\":\"0000-00-00 00:00:00\",\"svip\":\"0\",\"svip_time\":\"0000-00-00 00:00:00\",\"update_time\":\"2017-12-20 12:37:47\",\"master_level\":{\"level\":19,\"current\":[123000,442810],\"next\":[160000,602810],\"anchor_score\":598368,\"upgrade_score\":4442,\"sort\":2820},\"user_level\":45,\"color\":16746162,\"anchor_score\":598368},\"san\":12}}");
        }

        TEST_METHOD(InvalidRoomID)
        {
            // Testing invalid roomid.
            if (true)
            {
                auto info = Bili::Room::GetResolve(0);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsFalse(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                std::string error = info["error"];
                Assert::AreEqual("invalid roomid", error.c_str());
            }

            // Testing invalid roomid.
            if (true)
            {
                auto info = Bili::Room::GetInfo(0);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsFalse(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                std::string error = info["error"];
                Assert::AreEqual("invalid roomid", error.c_str());
            }

            // Testing invalid roomid.
            if (true)
            {
                auto info = Bili::Room::GetAnchorInfo(0);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsFalse(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                std::string error = info["error"];
                Assert::AreEqual("invalid roomid", error.c_str());
            }

            // Testing invalid roomid.
            if (true)
            {
                auto info = Bili::Room::GetRecentChat(0);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsFalse(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                std::string error = info["error"];
                Assert::AreEqual("invalid roomid", error.c_str());
            }
        }

        TEST_METHOD(GetServer)
        {
            auto info = Bili::Room::GetDanmakuServer(364513);
            Assert::IsTrue(info.is_object());
            Assert::IsTrue(info.find("error") == info.end());
            std::string chatid = info["chatid"];
            Assert::AreEqual(364513, std::stoi(chatid));
            Assert::IsTrue(info["dm_server_list"].is_array());
            json servers = info["dm_server_list"];
            std::string lastItem = servers.back();
            Assert::AreEqual("broadcastlv.chat.bilibili.com", lastItem.c_str());
#ifdef _DEBUG
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#endif
            for (auto &entry : servers)
            {
                Assert::IsTrue(entry.is_string());
#ifdef _DEBUG
                std::string content = entry;
                std::wstring name = converter.from_bytes(content);
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << name));
#endif
            }
        }

        TEST_METHOD(GetRoomResolve)
        {
            // Validating BiliGetRoomAnchorInfo
            if (true)
            {
                auto info = Bili::Room::GetResolve(364513);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsTrue(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                Assert::IsTrue(info["room_id"].is_number_integer());
                ROOM room = info["room_id"];
                Assert::AreEqual((ROOM)364513, room);
                Assert::IsTrue(info["uid"].is_number_integer());
                USER owner = info["uid"];
                Assert::AreEqual((USER)4278633, owner);
                Assert::IsTrue(info["encrypted"].is_boolean());
                bool isEncrypted = info["encrypted"];
                Assert::IsFalse(isEncrypted);
                Assert::IsTrue(info["is_hidden"].is_boolean());
                bool isHidden = info["is_hidden"];
                Assert::IsFalse(isHidden);
                Assert::IsTrue(info["is_locked"].is_boolean());
                bool isLocked = info["is_locked"];
                Assert::IsFalse(isLocked);
            }
        }

        TEST_METHOD(GetRoomInfo)
        {
            // Validating BiliGetRoomInfo.
            if (true)
            {
                auto info = Bili::Room::GetInfo(364513);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsTrue(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                Assert::IsTrue(info["uid"].is_number_integer());
                USER owner = info["uid"];
                Assert::AreEqual((USER)4278633, owner);
            }
        }

        TEST_METHOD(GetRoomAnchorInfo)
        {
            // Validating BiliGetRoomAnchorInfo
            if (true)
            {
                auto info = Bili::Room::GetAnchorInfo(364513);
                Assert::IsTrue(info.is_object());
                Assert::IsFalse(info["error"].is_string());
                Assert::IsTrue(info.is_object());
                // Validate the data object.
                Assert::IsTrue(info["info"].is_object());
                Assert::IsTrue(info["info"]["uid"].is_number_integer());
                Assert::IsTrue(info["level"].is_object());
                Assert::IsTrue(info["level"]["uid"].is_string());
                USER owner = info["info"]["uid"];
                Assert::AreEqual((USER)4278633, owner);
                std::string leveluid = info["level"]["uid"];
                Assert::AreEqual("4278633", leveluid.c_str());
            }
        }

        TEST_METHOD(GetRecentChat)
        {
            // Validating BiliGetRoomAnchorInfo
            if (true)
            {
                auto info = Bili::Room::GetRecentChat(1017);
                // Validate retrieval.
                Assert::IsTrue(info.is_object());
                Assert::IsTrue(info.find("error") == info.end());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(info.dump());
                Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
                // Validate the data object.
                Assert::IsTrue(info["room"].is_array());
                auto recent = info["room"];
                Assert::IsTrue(recent.size() > 0 && recent.size() <= 10);
                for (auto &entry : recent)
                {
                    Assert::IsTrue(entry.is_object());
                    Assert::IsTrue(entry.find("nickname") != entry.end());
                    Assert::IsTrue(entry.find("uid") != entry.end());
                    Assert::IsTrue(entry.find("text") != entry.end());
                    Assert::IsTrue(entry["nickname"].is_string());
                    Assert::IsTrue(entry["uid"].is_number_unsigned());
                    Assert::IsTrue(entry["text"].is_string());
#ifdef _DEBUG
                    std::wstring name = converter.from_bytes(entry["nickname"]);
                    std::size_t uid = entry["uid"];
                    std::wstring text = converter.from_bytes(entry["text"]);
                    Logger::WriteMessage(MSG(
                        __FUNCTION__ << " : " << name << "(" << uid << ")" <<
                        " says " << text
                    ));
#endif
                }
            }
        }

        TEST_METHOD(UserSendMessage)
        {
            Bili::User::Credentials cred = Bili::Settings::GetCredentials();
            Bili::User::SendOptions options;
            options.roomid = 364513;
            options.msg = L"testing\na\nreturn";
            options.prepare();
            auto response = Bili::User::SendRoomChat(cred, 364513, options);
            Assert::IsTrue(response.is_array());
            Assert::IsTrue(response.size() == 0);
        }
    };
}