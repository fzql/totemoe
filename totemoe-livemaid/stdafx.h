// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Bili Headers
#include "bili-settings/bili-settings.hpp"

// Buffer length.
#define MAX_LOADSTRING 1024
#define TOTEMOE_I18N_CONTRIVED_FILE_PATH L"totemoe-i18n.dll"
#define TOTEMOE_PLUGIN_CONTRIVED_FOLDER_PATH L"plugin\\"

// // Macros.
// #define TryLoadString(hInstance, uId, lpBuffer, cchBufferMax)                  \
//     if (!LoadStringW(hInstance, uId, lpBuffer, cchBufferMax))                  \
//     {                                                                          \
//         throw WinException(L"Load string failed");                             \
//     }

template <class T>
inline T WinGetLong(HWND hwnd, int which = GWL_USERDATA)
{
    return reinterpret_cast<T> (::GetWindowLong(hwnd, which));
}

template <class T>
inline void WinSetLong(HWND hwnd, T value, int which = GWL_USERDATA)
{
    ::SetWindowLong(hwnd, which, reinterpret_cast<long> (value));
}

// Reference additional headers.
#include "win-exception.hpp"
#include "i18n.hpp"
#include "Resource.h"
#include "resource-string.hpp"
#include "custom-message.hpp"
#include <string>


