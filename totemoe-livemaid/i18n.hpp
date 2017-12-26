
#pragma once

#include "stdafx.h"

class I18N
{
public:

    static void Free()
    {
        if (handle == nullptr)
        {
            if (!FreeLibrary(handle))
            {
                throw WinException(
                    L"FAILURE: Unable to unload the resource container");
            }
        }
    }

    static HMODULE GetHandle()
    {
        return handle;
    }

    static void Initialize()
    {
        handle = ::LoadLibraryEx(TOTEMOE_I18N_CONTRIVED_FILE_PATH,
            NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
        if (!handle)
        {
            throw WinException(
                L"FAILURE: Unable to load the resource container module");
        }
    }

private:

    static HMODULE handle;
};