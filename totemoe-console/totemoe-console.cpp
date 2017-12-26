// totemoe-console.cpp : Defines the entry point for the console application.
//

#include "stdafx.hpp"
#include "bili-server/bili-server.hpp"
#include "bili-https/bili-https.hpp"
#include <fstream>
// #include <windows.h>
// #include <stdio.h>


static bool sig_int = false;

BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
        sig_int = true;
        return TRUE;
    }
    return FALSE;
}

int main()
{
    bool done = false;
    std::string input;
    Bili::Server::Room room;

    // HANDLER
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    {
        std::cout << "ERROR: Could not set control handler\n";
        return 1;
    }

    // Set console unclickable.

    // ENTER PROGRAM LIFECYCLE.
    while (!done)
    {
        std::cout << "Enter Command: ";
        std::getline(std::cin, input);

        if (sig_int)
        {
            done = true;
            sig_int = false;
        }
        else if (input == "quit")
        {
            done = true;
        }
        else if (input == "help")
        {
            std::cout
                << "\nCommand List:\n"
                << "connect <room id>\n"
                << "close [<close reason>]\n"
                << "heartbeat\n"
                << "fetch\n"
                << "auto\n"
                << "show \n"
                << "help: Display this help text\n"
                << "quit: Exit the program\n"
                << std::endl;
        }
        else if (input.substr(0, 7) == "connect")
        {
            ROOM resolve;
            ROOM roomid;
            ROOM shortid;
            try
            {
                resolve = (ROOM)std::stoi(input.substr(8));

                auto preview = Bili::Room::GetResolve(resolve);
                if (preview.find("error") == preview.end())
                {
                    roomid = (ROOM)preview["room_id"].get<int>();
                    shortid = (ROOM)preview["short_id"].get<int>();
                }
                else
                {
                    roomid = resolve;
                }

                room.join(roomid);
            }
            catch (...)
            {
                std::cout << "> Invalid room id " << roomid << std::endl;
            }
        }
        else if (input.substr(0, 9) == "heartbeat")
        {
            room.heartBeat();
        }
        else if (input.substr(0, 5) == "close")
        {
            std::stringstream ss(input);

            std::string cmd;
            int close_code = websocketpp::close::status::normal;
            std::string reason;

            ss >> cmd >> close_code;
            std::getline(ss, reason);

            room.disconnect(close_code, reason);
        }
        else if (input.substr(0, 5) == "fetch")
        {
            auto messages = room.retrieve();
            std::cout << "\nFetched " << messages.size() << " message(s):\n";
        }
        else if (input.substr(0, 5) == "auto")
        {
            std::cout << "\nStarting to auto poll:\n";

            bool auto_poll_quit = false;
            while (!auto_poll_quit)
            {
                if (sig_int)
                {
                    auto_poll_quit = true;
                    sig_int = false;
                }
                auto metadata = room.getMetaData();
                if (metadata == nullptr)
                {
                    auto_poll_quit = true;
                    continue;
                }
                auto status = metadata->getStatus();
                if (status == "Closed")
                {
                    auto_poll_quit = true;
                    continue;
                }
                if (status == "Open")
                {
                    auto messages = room.retrieve();
                    if (!messages.empty())
                    {
                        time_t rawtime;
                        struct tm * timeinfo;
                        time(&rawtime);
                        timeinfo = localtime(&rawtime);
                        std::cout << " :: Fetched " << messages.size() << " message(s): " << asctime(timeinfo) << "\n";
                        for (auto &msg : messages)
                        {
                            std::cout << "    " << msg << "\n";
                        }
                    }
                }
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(50ms);
            }
        }
        else if (input.substr(0, 4) == "show")
        {
            std::cout << "\nConnected to room: " << room.getRoomID() << std::endl;
            Bili::Server::Connection::MetaData::ptr metadata = room.getMetaData();
            if (metadata)
            {
                std::cout << *metadata << std::endl;
            }
        }
        else
        {
            std::cout << "> Unrecognized Command" << std::endl;
        }
    }

    std::cout << "Stopping threads... Please wait...\n";
    return 0;
}

