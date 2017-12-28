// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BILISETTINGS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BILISETTINGS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#pragma once

#include "stdafx.hpp"

#include <simpleini/SimpleIni.h>

namespace Bili
{
    namespace Settings
    {
        namespace File
        {
            static CSimpleIniA ini;

            static SI_Error SetDefault(std::string const &section,
                std::string const &key, std::string const &value);

            BILISETTINGS_API SI_Error Load();

            BILISETTINGS_API SI_Error Save();

            BILISETTINGS_API char const *Get(std::string const &section,
                std::string const &key, char const *value = (char const *)NULL);

            BILISETTINGS_API std::wstring GetW(std::string const &section,
                std::string const &key, char const *value = (char const *)NULL);

            BILISETTINGS_API SI_Error Set(std::string const &section,
                std::string const &key, char const *value = (char const *)NULL);

            BILISETTINGS_API SI_Error SetW(std::string const &section,
                std::string const &key, std::wstring const &value = std::wstring());
        }

        BILISETTINGS_API std::string GetAPI(
            std::string const &server, std::string const &request);

        BILISETTINGS_API json GetCredentials();
    }
}