#include "stdafx.h"
#include "CppUnitTest.h"
#include "bili-server/bili-server.hpp"
#include "bili-settings/bili-settings.hpp"

#ifdef _DEBUG
#include <locale>
#include <codecvt>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace totemoeunittest
{
    TEST_CLASS(BiliServer)
    {
    public:

        TEST_METHOD(JoinRoomForever)
        {
            // Testing invalid roomid.
            if (true)
            {
                Bili::Settings::File::Load();
                Bili::Settings::File::Save();
                Bili::Server::Room room;
                room.join(364513);
            }
        }
    };
}