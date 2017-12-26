#include "stdafx.h"
#include "CppUnitTest.h"
#include "bili-settings/bili-settings.hpp"
#include <curl/curl.h>

#ifdef _DEBUG
#include <locale>
#include <codecvt>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace totemoeunittest
{
    TEST_CLASS(BiliSettings)
    {
    public:

        TEST_METHOD(LoadSettings)
        {
            // Testing invalid roomid.
            if (true)
            {
                Bili::Settings::File::Load();
                Bili::Settings::File::Save();
                std::string liveCAChain = Bili::Settings::File::Get("Security", "liveCertificateChain");
                Assert::AreEqual("api.live.bilibili.com.crt", liveCAChain.c_str());
#ifdef _DEBUG
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                // std::wstring wide = converter.from_bytes(liveEndPoint);
                // Logger::WriteMessage(MSG(__FUNCTION__ << " : " << wide));
#endif
            }
        }
    };
}