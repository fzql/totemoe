// bili-settings.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.hpp"
#include "bili-settings.hpp"
#include <sstream>
#include <codecvt>

LPCWSTR Bili::Settings::Get(
    std::wstring const &section, std::wstring const &key, LPCWSTR value)
{
    return File::inifile.GetValue(section.c_str(), key.c_str(), value);
}

SI_Error Bili::Settings::Set(
    std::wstring const &section, std::wstring const &key, LPCWSTR value)
{
    return File::inifile.SetValue(section.c_str(), key.c_str(), value);
}


std::wstring Bili::Settings::GetDefault(std::wstring const &section,
    std::wstring const &key)
{
    return File::def.GetValue(section.c_str(), key.c_str(), L"");
}

std::string Bili::Settings::GetAPI(
    std::wstring const &server, std::wstring const &request)
{
    File::Load();
    std::wstring useHttps = Get(L"Security", L"enforceHttps");
    std::wstring endPoint = Get(L"EndPoint", server);
    std::wstring resource;
    if (server == L"live")
    {
        resource = Get(L"LiveEndPoint", request);
    }
    std::wstringstream wss;
    wss << "http";
    if (useHttps == L"1")
    {
        wss << "s";
    }
    wss << "://" << endPoint << resource;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wss.str());
}

json Bili::Settings::GetCredentials()
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    File::Load();

    std::string a = converter.to_bytes(Get(L"Session", L"DedeUserID"));
    std::string b = converter.to_bytes(Get(L"Session", L"DedeUserID__ckMd5"));
    std::string c = converter.to_bytes(Get(L"Session", L"SESSDATA"));

    return json{
        { "DedeUserID", a },
        { "DedeUserID__ckMd5", b },
        { "SESSDATA", c }
    };
}
