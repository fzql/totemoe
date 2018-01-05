// bili-https-user.cpp : Defines the exported functions for the DLL application.

#include "stdafx.hpp"
#include "network.hpp"
#include "bili-https.hpp"
#include "bili-settings/bili-settings.hpp"


json Bili::User::GetSignInInfo(Credentials const &cred)
{
    auto url = Bili::Settings::GetAPI(L"live", L"signInInfo");
    return curlHttpsGet(url, {
        { "cookie", { { "content", cred } } },
        { "data", true }
    });
}

json Bili::User::SendRoomChat(Credentials const &cred,
    SendOptions const &options)
{
    auto url = Bili::Settings::GetAPI(L"live", L"userSendToRoom");
    return curlHttpsPost(url, {
        { "post", { { "content", options } } },
        { "cookie", { { "content", cred } } },
        { "data", true }
    });
}
