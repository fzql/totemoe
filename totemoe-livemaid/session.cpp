
#include "stdafx.h"
#include "resource-i18n.hpp"
#include "session.hpp"
#include <algorithm>
#include <codecvt>
#include <ctime>
#include <locale>

MessageSession::MessageSession() :
    m_bAutoReconnect(true), m_bStopThread(true), m_bLockVScroll(false),
    m_nNextMessageID(0), m_pFile(nullptr)
{
}

MessageSession::~MessageSession()
{
    if (!isClosed())
    {
        disconnect();
    }
    m_bStopThread = true;

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool MessageSession::isClosed() const
{
    auto status = m_room.getMetaData();
    if (status == nullptr)
    {
        return true;
    }
    else if (status->getStatus() == "Closed")
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MessageSession::isOpen() const
{
    auto status = m_room.getMetaData();
    if (status == nullptr)
    {
        return false;
    }
    else if (status->getStatus() == "Open")
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MessageSession::join(ROOM room)
{
    m_room.join(room);
}

void MessageSession::connect()
{
    m_room.connect();
    m_bStopThread = false;
    // ============================================ DETATCH A NEW THREAD ==== //
    // ==== The thread will forever attempt to fetch and parse messages. ==== //
    // ==== The thread is joined in the destructor of this class.        ==== //
    // ====================================================================== //
    m_thread = std::thread([this]() {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        auto roomid = getRoomID();

        bool quit = false;
        while (!quit)
        {
            auto messages = m_room.retrieve();
            if (!messages.empty())
            {
                if (m_pStatusBar != nullptr)
                {
                    m_pStatusBar->setText(2, L"MSG (" + std::to_wstring(m_vDisplay.size()) + L")");
                }
                for (auto &msg : messages)
                {
                    json data = json::parse(msg);
                    if (data.find("cmd") != data.end())
                    {
                        if (data["cmd"] == "cmd")
                        {
                            if (data["msg"] == "Closed" || data["msg"] == "Failed")
                            {
                                m_bStopThread = true;
                            }
                        }
                    }
                    parseMessage({
                        { "time" , time(nullptr) },
                        { "room" , m_room.getRoomID() },
                        { "data", data }
                    }, msg);
                }
            }
            if (m_bStopThread)
            {
                quit = true;
            }
            if (!quit)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(50ms);
            }
        }
    });
    if (m_thread.joinable())
    {
        m_thread.detach();
    }
}

void MessageSession::reconnect()
{
    disconnect();
    connect();
}

void MessageSession::disconnect()
{
    m_bStopThread = true;
    m_room.disconnect(websocketpp::close::status::normal, "User disconnect");
    // Close file.
    if (m_pFile != nullptr)
    {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
    // Shut down thread.
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(m_mutex_message);
    auto const &messages = m_room.retrieve();
    if (!messages.empty())
    {
        if (m_pStatusBar != nullptr)
        {
            m_pStatusBar->setText(2, L"MSG (" + std::to_wstring(m_vDisplay.size()) + L")");
        }
        for (auto &msg : messages)
        {
            parseMessage({
                { "time" , time(nullptr) },
                { "room" , m_room.getRoomID() },
                { "data", json::parse(msg) }
            }, msg);
        }
    }
    /*
    condition.wait(lock, [this]() {
        auto const &messages = m_room.retrieve();
        if (!messages.empty())
        {
            if (m_pStatusBar != nullptr)
            {
                m_pStatusBar->setText(2, L"MSG (" + std::to_wstring(m_vDisplay.size()) + L")");
            }
            for (auto &msg : messages)
            {
                parseMessage({
                    { "time" , time(nullptr) },
                    { "room" , m_room.getRoomID() },
                    { "data", json::parse(msg) }
                });
            }
        }
        return !messages.empty();
    });
    */
}

void MessageSession::setFilter(std::wstring const &keyword)
{
    m_keyword = keyword;
    m_bFilterRegex = false;
    m_regex = std::wregex(L"");
    m_vFiltered.clear();
    if (m_keyword.length() > 0)
    {
        for (size_t index = 0; index < m_vDisplay.size(); ++index)
        {
            bool match = false;
            for (size_t col = 0; !match && col < 9; ++col)
            {
                match = m_vDisplay[index][col].find(m_keyword) != std::wstring::npos;
            }
            if (match)
            {
                m_vFiltered.push_back(index);
            }
        }
    }
    // If the list view has been set,
    if (m_pTableListView != nullptr)
    {
        // Obtain its handle.
        HWND hListView = m_pTableListView->getHandle();
        // Next, set item count.
        int nItems = 0;
        if (m_keyword.length() == 0)
        {
            nItems = m_vDisplay.size();
        }
        else
        {
            nItems = m_vFiltered.size();
        }
        ListView_SetItemCountEx(hListView, nItems, LVSICF_NOINVALIDATEALL);
        // Scroll for the win!
        if (!m_bLockVScroll)
        {
            m_pTableListView->scrollToBottom();
        }
        if (m_pStatusBar != nullptr)
        {
            setFilterCount();
        }
    }
}

void MessageSession::setFilterRegex(std::wstring const &regex)
{
    m_keyword = regex;
    m_regex = std::wregex(regex);
    m_bFilterRegex = true;
    m_vFiltered.clear();
    if (m_keyword.length() > 0)
    {
        for (size_t index = 0; index < m_vDisplay.size(); ++index)
        {
            bool match = false;
            for (size_t col = 0; !match && col < 9; ++col)
            {
                match = std::regex_search(m_vDisplay[index][col], m_regex);
            }
            if (match)
            {
                m_vFiltered.push_back(index);
            }
        }
    }
    // If the list view has been set,
    if (m_pTableListView != nullptr)
    {
        // Obtain its handle.
        HWND hListView = m_pTableListView->getHandle();
        // Next, set item count.
        int nItems = 0;
        if (m_keyword.length() == 0)
        {
            nItems = m_vDisplay.size();
        }
        else
        {
            nItems = m_vFiltered.size();
        }
        ListView_SetItemCountEx(hListView, nItems, LVSICF_NOINVALIDATEALL);
        // Scroll for the win!
        if (!m_bLockVScroll)
        {
            m_pTableListView->scrollToBottom();
        }
        if (m_pStatusBar != nullptr)
        {
            setFilterCount();
        }
    }
}

void MessageSession::setFilterCount()
{
    // If keyword is empty, notify user that filter has been reset.
    if (m_keyword.length() == 0)
    {
        ResourceString msg(I18N::GetHandle(), IDS_STATUS_FILTERRESET);
        m_pStatusBar->setText(0, (LPCWSTR)msg);
    }
    // Otherwise, notify number of results and the keyword
    else
    {
        WCHAR status[MAX_LOADSTRING];
        // Retrieve format string.
        int nCount = m_vFiltered.size();
        if (m_bFilterRegex)
        {
            ResourceString fmt(I18N::GetHandle(), IDS_STATUS_FILTERRESULT_REGEX);
            _swprintf_p(status, MAX_LOADSTRING, (LPCWSTR)fmt,
                nCount, m_keyword.c_str());
        }
        else
        {
            ResourceString fmt(I18N::GetHandle(), IDS_STATUS_FILTERRESULT);
            _swprintf_p(status, MAX_LOADSTRING, (LPCWSTR)fmt,
                nCount, m_keyword.c_str());
        }
        m_pStatusBar->setText(0, status);
    }
}

void MessageSession::parseMessage(json const &object, std::string const &raw)
{
    // Converter between UTF-8 (std::string) and UTF-16 (std::wstring).
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // Preferences: Time Zone.
    std::wstring timeZone = Bili::Settings::Get(L"General", L"timeZone");
    // Preferences: TXT or CSV output.
    std::wstring autoExport = Bili::Settings::Get(L"Danmaku", L"autoExport");
    bool exportMessage = autoExport == L"on" || autoExport == L"csv";
    // Separator for TXT / CSV output.
    char separator = '\t';
    if (autoExport == L"csv")
    {
        separator = ',';
    }

    // Flag to set if Danmaku is generated due to SmallTV raffle.
    bool isSmallTVRaffle = false;

    // ========================================== FIRST ROUND OF PARSING ==== //
    // ==== Parse the server protocols and save data into the database ====== //
    // ====================================================================== //
    json value = {
        { "mID", m_nNextMessageID },
        { "room", object["room"] },
        { "time", object["time"] }
    };
    const json &data = object["data"];
    // Obtain the server-side protocol name.
    std::string serverProtocol = data["cmd"];
    if (serverProtocol == "WEBSOCKET")
    {
        value["protocol"] = "WebSocket";
        const json &info = data["msg"];

        if (info == "Connecting")
        {
            value["info"] = ResourceString(I18N::GetHandle(), IDS_WEBSOCKET_CONNECTING);
        }
        else if (info == "Established")
        {
            value["info"] = ResourceString(I18N::GetHandle(), IDS_WEBSOCKET_ESTABLISHED);
        }
        else if (info == "Closed")
        {
            value["info"] = ResourceString(I18N::GetHandle(), IDS_WEBSOCKET_CLOSED);
        }
        else if (info == "Failed")
        {
            value["info"] = ResourceString(I18N::GetHandle(), IDS_WEBSOCKET_FAILED);
        }
        else
        {
            value["info"] = ResourceString(I18N::GetHandle(), IDS_WEBSOCKET_UNKNOWN);
        }

        // value["info"] = data["msg"];
    }
    else if (serverProtocol == "DANMU_MSG")
    {
        value["protocol"] = "Danmaku";
        value["info"] = data["info"];

        const json &info = data["info"];
        value["user"] = {
            { "id", info[2][0] },
            { "name", info[2][1] }
        };
        value["content"] = info[1];

        // Detect if danmaku is generated from SmallTV Raffle.
        if (value["content"].get<std::string>() ==
            "- ( \xE3\x82\x9C- \xE3\x82\x9C)\xE3\x81\xA4\xE3\x83\xAD"
            " \xE4\xB9\xBE\xE6\x9D\xAF~ - bilibili")
        {
            if (raw.find("{\"cmd\":\"DANMU_MSG\"") == 0)
            {
                isSmallTVRaffle = true;
            }
        }
    }
    else if (serverProtocol == "SEND_GIFT")
    {
        value["protocol"] = "Gifting";
        value["info"] = data["data"];

        const json &info = data["data"];
        value["user"] = {
            { "id", info["uid"] },
            { "name", info["uname"] }
        };
        value["gift"] = {
            { "id", info["giftId"] },
            { "name", info["giftName"] },
            { "action", info["action"] },
            { "price", info["price"] }
        };
        value["count"] = info["num"];
    }
    else if (serverProtocol == "SYS_MSG")
    {
        value["protocol"] = "Announcement";
        value["info"] = data.dump();

        const json &info = data;
        value["content"] = data["msg"];
        // Turn HTTP into HTTPS for security.
        std::string url = data["url"];
        std::regex regex_http("^(http):\\/\\/");
        url = std::regex_replace(url, regex_http, "https");
        value["url"] = url;
        value["rep"] = data["rep"].get<int>();
    }
    else
    {
        value["protocol"] = "Unknown";
        value["info"] = data;
    }

    // Preferences: Do we ignore this message?
    std::string protocol = value["protocol"];
    int filterLevel;
    if (protocol == "WebSocket")
    {
        // Do not block WebSocket protocol.  Always shown.
        filterLevel = 3;
    }
    else if (protocol == "Danmaku")
    {
        filterLevel = std::stoi(Bili::Settings::Get(L"Danmaku", L"filterDanmaku"));
    }
    else if (protocol == "Gifting")
    {
        filterLevel = std::stoi(Bili::Settings::Get(L"Danmaku", L"filterGifting"));
    }
    else if (protocol == "Announcement")
    {
        filterLevel = std::stoi(Bili::Settings::Get(L"Danmaku", L"filterAnnouncement"));
    }
    else
    {
        filterLevel = std::stoi(Bili::Settings::Get(L"Danmaku", L"filterUnknown"));
    }

    // Ignore this message of filterLevel is 0.
    if (filterLevel == 0)
    {
        return;
    }

    // Increment message ID if message is not ignored.
    m_nNextMessageID += 1;

    // ================================================ SAVE TO DATABASE ==== //
    // ==== TODO ============================================================ //
    // ====================================================================== //

    // Do not display this message if filterLevel is 1.
    if (filterLevel == 1)
    {
        return;
    }
    // Do not display this message if it is generated by SmallTV Raffle.
    if (isSmallTVRaffle)
    {
        std::wstring s = Bili::Settings::Get(L"Danmaku", L"filterSmallTV");
        if (s == L"on")
        {
            return;
        }
    }

    // ======================================= GENERATE LISTVIEW DISPLAY ==== //
    // ==== Generate 9 columms of display text to fill the list view ======== //
    // ====================================================================== //
    std::array<std::wstring, 9> display;
    display[0] = std::to_wstring(value["room"].get<ROOM>());
    display[1] = std::to_wstring(value["mID"].get<long>());
    {
        std::time_t time = value["time"].get<std::time_t>();
        std::tm* t;
        if (timeZone == L"system")
        {
            t = std::localtime(&time);
        }
        else
        {
            int offset = 0;
            // Regex to match offset hours and minutes
            std::wregex regexUTC(L"\\(UTC(?:([+-]?[0-9]{1,2}?):([0-5][0-9]))\\)");
            std::wsmatch match;
            if (std::regex_search(timeZone, match, regexUTC))
            {
                int hour = std::stoi(match[1]);
                int minute = std::stoi(match[2]);
                offset = (hour * 60 + minute) * 60;
            }
            time += offset;
            t = std::gmtime(&time);
        }

        // If message is on same day, use existing file.
        bool createNewFile = exportMessage && (
            m_pFile == nullptr ||
            m_exportSetting != autoExport ||
            t->tm_year != m_tRecent.tm_year ||
            t->tm_mon != m_tRecent.tm_mon ||
            t->tm_mday != m_tRecent.tm_mday);
        if (createNewFile)
        {
            std::string name;
            {
                std::stringstream sst;
                if (autoExport == L"csv")
                {
                    sst << std::put_time(t, "history\\%Y-%m-%d.csv");
                }
                else
                {
                    sst << std::put_time(t, "history\\%Y-%m-%d.txt");
                }
                name = sst.str();
                m_exportSetting = autoExport;
            }
            try
            {
                // Create a new "history" folder in case it does not exist.
                // Otherwise, the file open statement below would not work.
                BOOL code = CreateDirectory(L"history", NULL);
                if (code == 0)
                {
                    // Failed to creat directory / directory already exists.
                    // Reference: https://msdn.microsoft.com/en-us/library/windows/desktop/aa363855(v=vs.85).aspx
                    if (GetLastError() != ERROR_ALREADY_EXISTS)
                    {
                        throw WinException(L"Cannot create \"history\" folder");
                    }
                }
                // For files, work directly using C functionality because they
                // are faster and locale-unaware.  You can't imagine how hard it
                // is to output WCHAR (UTF-16) text into UTF-8 text files.  More
                // than 10 hours has been wasted on trying to get std::wofstream
                // to work.  If you can use std::wofstream and get the correct
                // behavior, please submit a pull request.
                m_pFile = fopen(name.c_str(), "a+b");
                // std::locale::global(std::locale(""));
                if (m_pFile == nullptr)
                {
                    throw WinException(L"Cannot open file");
                }
                // Peek if file is empty.
                // Reference: https://stackoverflow.com/a/13566193/1377770
                fseek(m_pFile, 0, SEEK_END);
                int size = ftell(m_pFile);
                // If empty, create file with the new date.
                if (size == 0)
                {
                    // Microsoft Excel needs BOM to decode UTF-8 CSV.
                    // Reference: https://stackoverflow.com/a/11399444/1377770
                    std::string bom("\xEF\xBB\xBF");
                    fwrite(bom.data(), sizeof(char), bom.size(), m_pFile);
                    // Output the header row upon creation of new file.
                    std::wstringstream wss;
                    wss << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_ROOM)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_MSGID)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_TIME)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_PROTOCOL)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_USERID)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_USERTYPE)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_USERNAME)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_BILI_SUMMARY)
                        << separator << (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_LIVEMAID_TRIGGER);
                    writeLine(wss.str());
                }
            }
            catch (...)
            {
                throw WinException(L"Cannot open file");
            }
        }
        else
        {
            // TODO: Read last message ID.
        }
        m_tRecent = *t;

        std::wstringstream wss;
        wss << std::put_time(t, L"%Y/%m/%d %H:%M:%S");
        display[2] = wss.str();
    }
    if (value["protocol"] == "Unknown")
    {
        display[3] = (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_PROTOCOL_UNKNOWN);
        display[7] = converter.from_bytes(value["info"].dump());
    }
    else if (value["protocol"] == "WebSocket")
    {
        display[3] = (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_PROTOCOL_PORT);
        display[7] = converter.from_bytes(value["info"].get<std::string>());
    }
    else if (value["protocol"] == "Danmaku")
    {
        display[3] = (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_PROTOCOL_DANMAKU);
        display[4] = std::to_wstring(value["user"]["id"].get<USER>());
        display[6] = converter.from_bytes(value["user"]["name"].get<std::string>());
        display[7] = converter.from_bytes(value["content"].get<std::string>());
    }
    else if (value["protocol"] == "Gifting")
    {
        display[3] = (LPCWSTR)ResourceString(I18N::GetHandle(), IDS_PROTOCOL_GIFTING);
        display[4] = std::to_wstring(value["user"]["id"].get<USER>());
        display[6] = converter.from_bytes(value["user"]["name"].get<std::string>());
        {
            std::wstringstream wss;
            wss << converter.from_bytes(value["gift"]["action"].get<std::string>())
                << " "
                << converter.from_bytes(value["gift"]["name"].get<std::string>());
            if (value["count"] > 1)
            {
                wss << "x" << value["count"].get<long>();
            }
            display[7] = wss.str();
        }
    }
    else if (value["protocol"] == "Announcement")
    {
        display[7] = converter.from_bytes(value["content"].get<std::string>());
    }
    m_vDisplay.push_back(display);

    // Save message to file.
    if (m_pFile != nullptr && exportMessage)
    {
        std::wstringstream wss;
        for (size_t col = 0; col < display.size(); ++col)
        {
            wss << display[col];
            if (col + 1 < display.size())
            {
                wss << separator;
            }
        }
        writeLine(wss.str());
    }

    if (m_keyword.length() > 0)
    {
        bool match = false;
        for (size_t col = 0; !match && col < 9; ++col)
        {
            match = display[col].find(m_keyword) != std::wstring::npos;
        }
        if (match)
        {
            m_vFiltered.push_back(m_vDisplay.size() - 1);
        }
    }

    if (m_pTableListView != nullptr)
    {
        HWND hListView = m_pTableListView->getHandle();
        if (m_vFiltered.empty())
        {
            ListView_SetItemCountEx(hListView, m_vDisplay.size(),
                LVSICF_NOINVALIDATEALL);
        }
        else
        {
            ListView_SetItemCountEx(hListView, m_vFiltered.size(),
                LVSICF_NOINVALIDATEALL);
        }
        if (!m_bLockVScroll)
        {
            m_pTableListView->scrollToBottom();
        }
        if (m_pStatusBar != nullptr)
        {
            if (m_keyword.length() > 0)
            {
                setFilterCount();
            }
        }
    }
}

void MessageSession::writeLine(std::wstring const &content)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string bytes = converter.to_bytes(content + L"\r\n");
    if (m_pFile != nullptr)
    {
        fwrite(bytes.data(), sizeof(char), bytes.size(), m_pFile);
        fflush(m_pFile);
    }
}
