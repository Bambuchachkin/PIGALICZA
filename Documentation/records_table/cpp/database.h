#pragma once
#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include <mysql_connection.h>

namespace sql {
    class Connection;
}

class Database {
private:
    std::unique_ptr<sql::Connection> con;
public:
    Database(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
    ~Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    std::pair<int, std::string> registerUser(const std::string& username, const std::string& password);
    std::pair<int, std::string> loginUser(const std::string& username, const std::string& password, std::string& out_session_id);

    int getUserIdFromSession(const std::string& session_id);
    std::string getUserBySession(const std::string& session_id);
    std::string getVerificationCodeBySession(const std::string& session_id);
    void logout(const std::string& session_id);

    std::string getLeaderboard(const std::string& game);
    sql::Connection* getConnection() { return con.get(); }
};