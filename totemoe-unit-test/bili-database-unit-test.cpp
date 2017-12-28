#include "stdafx.h"
#include "CppUnitTest.h"
#ifndef _DEBUG
#include "bili-database/bili-database.hpp"
#endif

#ifdef _DEBUG
#include <locale>
#include <codecvt>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace totemoeunittest
{
#ifndef _DEBUG
    TEST_CLASS(BiliDatabase)
    {
    public:

        TEST_METHOD(CreateTable)
        {
            // Testing invalid roomid.
            if (true)
            {
                Assert::IsTrue(Bili::Database::MySQL::Load());

                Assert::AreEqual(
                    Bili::Settings::File::Get("MySQL", "schema"),
                    Bili::Database::MySQL::schema.c_str()
                );

                Assert::IsTrue(Bili::Settings::File::Save() != SI_Error::SI_FAIL);
            }
        }
    };
#endif
}