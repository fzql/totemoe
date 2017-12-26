// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BILIDATABASE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BILIDATABASE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "stdafx.hpp"
#include "bili-settings/bili-settings.hpp"
#include <mysql/mysql_connection.h>
#include <mysql/mysql_driver.h>
#include <mysql/cppconn/driver.h>
#include <mysql/cppconn/exception.h>
#include <mysql/cppconn/resultset.h>
#include <mysql/cppconn/statement.h>
#include <mysql/cppconn/prepared_statement.h>
#include <sstream>

namespace Bili
{
    namespace Database
    {
        namespace MySQL
        {
            static sql::mysql::MySQL_Driver *driver;

            boost::shared_ptr<sql::Connection> connection;

            static sql::SQLString schema;

            static bool Load()
            {
                Bili::Settings::File::Load();

                if (strcmp(Bili::Settings::File::Get("Security", "enableMySQL"), "1") != 0)
                {
                    return false;
                }
                if (driver != nullptr)
                {
                    return false;
                }

                std::stringstream sst;
                sql::PreparedStatement *statement;
                std::string uri, query;

                sql::SQLString host = Bili::Settings::File::Get("MySQL", "host");
                sql::SQLString port = Bili::Settings::File::Get("MySQL", "port");
                sql::SQLString user = Bili::Settings::File::Get("MySQL", "appUsername");
                sql::SQLString pass = Bili::Settings::File::Get("MySQL", "appPassword");

                sst << "tcp://" << host << ":" << port;
                uri = sst.str();
                sst.str("");

                /* Create a connection */
                try
                {
                    driver = sql::mysql::get_mysql_driver_instance();
                }
                catch (sql::SQLException e)
                {
                    driver = nullptr;
                    return false;
                }
                try
                {
                    sql::SQLString uri = uri.c_str();
                    connection.reset(driver->connect(uri, user, pass));
                }
                catch (sql::SQLException e)
                {
                    connection = nullptr;
                    return false;
                }
                catch (...)
                {
                    return false;
                }

                /* Connect to the MySQL test database */
                schema = Bili::Settings::File::Get("MySQL", "schema");
                connection->setSchema(schema);

                try // To create the User table.
                {
                    sst << "CREATE TABLE IF NOT EXISTS `" << schema << "`.`User`"
                        << " (`idUser` BIGINT UNSIGNED NOT NULL,"
                        << " `Name` NVARCHAR(80) NOT NULL,"
                        << " `Time` TIME NOT NULL,"
                        << " PRIMARY KEY (`idUser`))"
                        << " DEFAULT CHARACTER SET = utf8";
                    query = sst.str();
                    sst.str("");
                    statement = connection->prepareStatement(query.c_str());
                    statement->executeQuery();
                    delete statement;
                }
                catch (sql::SQLException e) { return false; }

                try // To create the History table.
                {
                    sst << "CREATE TABLE IF NOT EXISTS `" << schema << "`.`Message`"
                        << " (`idMsg` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,"
                        << " `Time` TIME NOT NULL,"
                        << " `Protocol` NVARCHAR(30) NOT NULL,"
                        << " `idRoom` BIGINT UNSIGNED,"
                        << " `idUser` BIGINT UNSIGNED,"
                        << " `Name` NVARCHAR(80),"
                        << " `Summary` NVARCHAR(256),"
                        << " `Object` JSON,"
                        << " PRIMARY KEY (`idMsg`),"
                        << " FOREIGN KEY (`idUser`) REFERENCES `User`(`idUser`))"
                        << " DEFAULT CHARACTER SET = utf8";
                    query = sst.str();
                    sst.str("");
                    statement = connection->prepareStatement(query.c_str());
                    statement->executeQuery();
                    delete statement;
                }
                catch (sql::SQLException e) { return false; }

                try // To create the History table.
                {
                    sst << "CREATE TABLE IF NOT EXISTS `" << schema << "`.`Room`"
                        << " (`idRoom` BIGINT UNSIGNED NOT NULL,"
                        << " `idShort` BIGINT UNSIGNED NOT NULL,"
                        << " `idOwner` BIGINT UNSIGNED NOT NULL,"
                        << " `TimeRecorded` TIME NOT NULL,"
                        << " `Medal` NCHAR(12),"
                        << " PRIMARY KEY (`idRoom`),"
                        << " UNIQUE KEY (`idShort`),"
                        << " FOREIGN KEY (`idOwner`) REFERENCES `User`(`idUser`))"
                        << " DEFAULT CHARACTER SET = utf8";
                    query = sst.str();
                    sst.str("");
                    statement = connection->prepareStatement(query.c_str());
                    statement->executeQuery();
                    delete statement;
                }
                catch (sql::SQLException e) { return false; }

                try // To create the History table.
                {
                    sst << "CREATE TABLE IF NOT EXISTS `" << schema << "`.`Gift`"
                        << " (`idGift` BIGINT UNSIGNED NOT NULL,"
                        << " `Name` NVARCHAR(30) NOT NULL,"
                        << " `Price` BIGINT UNSIGNED NOT NULL,"
                        << " `TimeRecorded` TIME NOT NULL,"
                        << " PRIMARY KEY (`idGift`))"
                        << " DEFAULT CHARACTER SET = utf8";
                    query = sst.str();
                    sst.str("");
                    statement = connection->prepareStatement(query.c_str());
                    statement->executeQuery();
                    delete statement;
                }
                catch (sql::SQLException e) { return false; }

                try // To create the History table.
                {
                    sst << "CREATE TABLE IF NOT EXISTS `" << schema << "`.`Gifting`"
                        << " (`idMsg` BIGINT UNSIGNED NOT NULL,"
                        << " `idGift` BIGINT UNSIGNED NOT NULL,"
                        << " `getsMedal` BOOL NOT NULL,"
                        << " `giftName` NVARCHAR(30) NOT NULL,"
                        << " `giftPrice` BIGINT UNSIGNED NOT NULL,"
                        << " `giftCount` BIGINT UNSIGNED NOT NULL,"
                        << " UNIQUE KEY (`idMsg`),"
                        << " FOREIGN KEY (`idMsg`) REFERENCES `Message`(`idMsg`),"
                        << " FOREIGN KEY (`idRoom`) REFERENCES `Room`(`idRoom`),"
                        << " FOREIGN KEY (`idGift`) REFERENCES `Gift`(`idGift`))"
                        << " DEFAULT CHARACTER SET = utf8";
                    query = sst.str();
                    sst.str("");
                    statement = connection->prepareStatement(query.c_str());
                    statement->executeQuery();
                    delete statement;
                }
                catch (sql::SQLException e) { return false; }

                return true;
            }

            static bool Close()
            {
                auto con = connection.get();
                if (con != nullptr)
                {
                    delete con;
                    con = nullptr;
                }
                connection.reset();
            }
        }
    }
}