
#pragma once

#include "stdafx.h"
#include "controller.hpp"
#include <codecvt>

class ResourceString final
{

    enum { MAX_RESSTRING = MAX_LOADSTRING };
public:

    ResourceString(HINSTANCE hInstance, int resId)
    {
        /*
        if (!::FindResource(resContainer, MAKEINTRESOURCE(resId), RT_STRING))
        {
            throw WinException(L"FAILURE: Unable to load the string table");
        }
        */
        if (::LoadString(hInstance, resId, m_buf, MAX_RESSTRING + 1))
        {
        }
        else if (::LoadString(I18N::GetHandle(), resId, m_buf, MAX_RESSTRING + 1))
        {
        }
        else
        {
            throw WinException(L"FAILURE: Unable to load the resource string (" +
                std::to_wstring(resId) + L")");
        }
    };

    operator LPCWSTR () const
    {
        return m_buf;
    }

public:
#ifdef NLOHMANN_JSON_HPP
    friend void to_json(json &j, ResourceString const &r)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        j = converter.to_bytes(std::wstring(r.m_buf));
    }
#endif
private:

    WCHAR m_buf[MAX_RESSTRING + 1];
};
