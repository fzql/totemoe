// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef BILISETTINGS_EXPORTS
#define BILISETTINGS_API __declspec(dllexport)
#else
#define BILISETTINGS_API __declspec(dllimport)
#endif

#include "targetver.hpp"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here

#include "json/json.hpp"

typedef nlohmann::json json;