#pragma once
#include <iostream>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <nlohmann/json.hpp>

class db_writer {
private:
    const std::string server = "tcp://127.0.0.1:3306";
    const std::string username = "my_user";
    const std::string password = "aboba123";
    const std::string database = "pigalicza_db";

    sql::mysql::MySQL_Driver *driver;
    std::unique_ptr<sql::Connection> con;
public:
    db_writer();
    ~db_writer() = default;
    void process_command(nlohmann::json command);
};