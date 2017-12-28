// bili-https-user.cpp : Defines the exported functions for the DLL application.

#include "stdafx.hpp"
#include "network.hpp"
#include "bili-https.hpp"
#include "bili-settings/bili-settings.hpp"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

json Bili::User::GetSignInInfo(Credentials const &cred)
{
    auto url = Bili::Settings::GetAPI("live", "signInInfo");
    return curlHttpsGet(url, {
        { "cookie", { { "content", cred } } },
        { "data", true }
    });
}

json Bili::User::SendRoomChat(Credentials const &cred,
    SendOptions const &options)
{
    auto url = Bili::Settings::GetAPI("live", "userSendToRoom");
    return curlHttpsPost(url, {
        { "post", { { "content", options } } },
        { "cookie", { { "content", cred } } },
        { "data", true }
    });
}
