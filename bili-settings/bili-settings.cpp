// bili-settings.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-settings.hpp"
#include <sstream>

std::string Bili::Settings::GetAPI(
    std::string const &server, std::string const &request)
{
    File::Load();
    std::string useHttps = File::Get("Security", "enforceHttps");
    std::string endPoint = File::Get("EndPoint", server);
    std::string resource;
    if (server == "live")
    {
        resource = File::Get("LiveEndPoint", request);
    }
    std::stringstream sst;
    sst << "http";
    if (useHttps == "1")
    {
        sst << "s";
    }
    sst << "://" << endPoint << resource;
    return sst.str();
}

json Bili::Settings::GetCredentials()
{
    File::Load();
    return json{
        { "DedeUserID", File::Get("Credentials", "DedeUserID") },
        { "DedeUserID__ckMd5", File::Get("Credentials", "DedeUserID__ckMd5") },
        { "SESSDATA", File::Get("Credentials", "SESSDATA") }
    };
}