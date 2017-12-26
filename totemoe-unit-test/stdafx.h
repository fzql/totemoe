// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
//! Enable easy construction of assert messages using \c std::wstringstream.
#define MSG(msg) [&] { std::wstringstream _s; _s << msg; return _s.str(); }().c_str()