
#pragma once

#include "stdafx.h"

// The exception class: stores the message and the error code
class WinException
{
public:

    WinException(std::wstring const &message) :
        m_error(::GetLastError()), m_message(message)
    {}

    DWORD GetError() const { return m_error; }

    LPCWSTR GetMessage() const { return m_message.c_str(); }
private:

    DWORD m_error;

    std::wstring m_message;
};

// The out-of-memory handler: throws exception
/*
int NewHandler(size_t size)
{
    throw WinException(L"Out of memory");
    return 0;
}
*/