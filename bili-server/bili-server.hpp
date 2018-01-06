// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BILISERVER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BILISERVER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "stdafx.hpp"
// #include <libwebsockets/libwebsockets.h>
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <chrono>
#include <mutex>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <asio/asio.hpp>
//#include <libuv/uv.h>

namespace Bili
{
    namespace Server
    {
        typedef websocketpp::client<websocketpp::config::asio_client> wsclient;

        namespace Connection
        {
            class MetaData final
            {
            public:

                typedef websocketpp::lib::shared_ptr<MetaData> ptr;
            public:

                MetaData(int id, websocketpp::connection_hdl hdl, 
                    std::string uri, std::string command);
            public:

                void on_open(wsclient *c, websocketpp::connection_hdl hdl);

                void on_close(wsclient *c, websocketpp::connection_hdl hdl);

                void on_fail(wsclient *c, websocketpp::connection_hdl hdl);

                void on_message(websocketpp::connection_hdl hdl, wsclient::message_ptr msg);

                int getID() const
                {
                    return m_id;
                }

                websocketpp::connection_hdl getHandle() const
                {
                    return m_hdl;
                }

                std::string getStatus() const
                {
                    return m_status;
                }

                void setCommand(std::string const &command)
                {
                    m_command = command;
                }

                std::vector<std::string> retrieve();

            public:

                BILISERVER_API friend std::ostream & operator<< (std::ostream & out, MetaData const &data);
            private:

                int m_id;

                websocketpp::connection_hdl m_hdl;

                std::string m_status;

                std::string m_uri;

                std::string m_server;

                size_t m_message_count;

                size_t m_audience;

                std::string m_error_reason;

                std::string m_command;

                std::vector<std::string> m_messages;

                std::mutex m_mutex_messages;
            };

            class EndPoint final
            {
            public:

                EndPoint() : m_next_id(0)
                {
                    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
                    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

                    m_endpoint.init_asio();
                    m_endpoint.start_perpetual();

                    m_thread.reset(new websocketpp::lib::thread(&wsclient::run, &m_endpoint));
                }

                BILISERVER_API ~EndPoint();
            public:

                int connect(std::string const &uri, ROOM roomid);

                void disconnect(int id, websocketpp::close::status::value code, std::string reason);

                void send(int id, std::string message);

                MetaData::ptr getMetaData(int id) const;

                std::vector<std::string> retrieve(int id);
            private:

                typedef std::map<int, MetaData::ptr> con_list;

                wsclient m_endpoint;

                websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

                std::size_t m_audience_count;

                con_list m_connection_list;

                int m_next_id;

                ROOM m_roomid;
            };
        }

        BILISERVER_API std::string GetURI(ROOM roomid);

        class Room
        {
        public:

            Room() : m_roomid(-1) {}

            ~Room()
            {
                try
                {
                    m_heart_beat.join();
                }
                catch (const std::system_error &)
                {
                    /* suppressed */
                }
            }
        public:

            BILISERVER_API void join(ROOM room);

            BILISERVER_API void send(std::string const &msg);

            BILISERVER_API void connect();

            BILISERVER_API void disconnect(websocketpp::close::status::value code, std::string const &reason);

            BILISERVER_API void heartBeat();

            BILISERVER_API void clear();

            BILISERVER_API std::vector<std::string> retrieve();

            BILISERVER_API ROOM getRoomID() const { return m_roomid; }

            BILISERVER_API Connection::MetaData::ptr getMetaData() const
            {
                return m_endpoint.getMetaData(m_id);
            }
        public:

            static std::string PacketHello(std::string const &command);

            static std::string PacketHeart(std::string const &command);
        private:
            // Client endpoint
            Connection::EndPoint m_endpoint;
            // Timer for heartbeat
            std::thread m_heart_beat;
            // Room id
            size_t m_roomid;
            // Connection id
            size_t m_id;
        };

        // void sendHeartBeatToRoom(Room &room);
    }
}
