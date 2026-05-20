#include <iostream>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <nlohmann/json.hpp>

#include "db_writer.h"

int main() {
    nlohmann::json command;
    command["type"] = "record"; //"record" to write new record / "v_code" to change user
    command["v_code"] = "73720"; //BIBA
    command["t_s"] = "12"; //tanks score
    command["t_t"] = "120";
    command["r_s"] = "10"; //races score
    command["r_t"] = "100";

    db_writer writer;

    try {
        writer.process_command(command);

    } catch (sql::SQLException &e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        std::cerr << "ERRROR CODE: " << e.getErrorCode() << std::endl;
        return 1;
    }

    return 0;
}