#include "database.h"
#include "functions.h"
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <iostream>
#include <ctime>

using json = nlohmann::json;

Database::Database(const std::string& host, const std::string& user,
                   const std::string& pass, const std::string& db)
{
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    con.reset(driver->connect(host, user, pass));
    con->setSchema(db);
    std::cout << "Успешное подключение к MySQL!" << std::endl;
}

Database::~Database() {
    // unique_ptr сам всё закроет
}

std::pair<int, std::string> Database::registerUser(const std::string& username, const std::string& password) {
    try {
        if (username.empty() || password.empty())
            return {400, R"({"message":"Username and password required"})"};
        if (password.size() < 6)
            return {400, R"({"message":"Password too short"})"};

        std::string password_hash = hash_password(password);

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO users (username, password_hash) VALUES (?, ?)"));
        pstmt->setString(1, username);
        pstmt->setString(2, password_hash);
        pstmt->executeUpdate();

        return {201, R"({"message":"Registered"})"};
    } catch (sql::SQLException& e) {
        if (e.getErrorCode() == 1062)
            return {409, R"({"message":"Username exists"})"};
        std::cerr << "DB error in register: " << e.what() << std::endl;
        return {500, R"({"message":"DB error"})"};
    } catch (...) {
        return {500, R"({"message":"Server error"})"};
    }
}

std::pair<int, std::string> Database::loginUser(const std::string& username, const std::string& password,
                                                std::string& out_session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT id, password_hash FROM users WHERE username = ?"));
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next())
            return {401, R"({"message":"Invalid username or password"})"};

        int user_id = res->getInt("id");
        std::string stored_hash = res->getString("password_hash");

        if (!verify_password(password, stored_hash))
            return {401, R"({"message":"Invalid username or password"})"};

        out_session_id = generate_session_id();

        std::unique_ptr<sql::PreparedStatement> ins_sess(
            con->prepareStatement("INSERT INTO sessions (session_id, user_id, expires_at) VALUES (?, ?, DATE_ADD(NOW(), INTERVAL 1 DAY))"));
        ins_sess->setString(1, out_session_id);
        ins_sess->setInt(2, user_id);
        ins_sess->executeUpdate();

        return {200, R"({"message":"Login successful"})"};
    } catch (sql::SQLException& e) {
        std::cerr << "Login error: " << e.what() << std::endl;
        return {500, R"({"message":"DB error"})"};
    } catch (...) {
        return {500, R"({"message":"Server error"})"};
    }
}

std::string Database::getUserBySession(const std::string& session_id) {
    int user_id = getUserIdFromSession(session_id);
    if (user_id == 0) return "";

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT username FROM users WHERE id = ?"));
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next())
            return res->getString("username");
    } catch (...) {}
    return "";
}

void Database::logout(const std::string& session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("DELETE FROM sessions WHERE session_id = ?"));
        pstmt->setString(1, session_id);
        pstmt->executeUpdate();
    } catch (...) {}
}

std::string Database::getStats() {
    json arr = json::array();
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT ip_address, visit_time FROM visits ORDER BY id DESC LIMIT 10"));
        while (res->next()) {
            json obj;
            obj["ip"] = res->getString("ip_address");
            obj["visit_time"] = res->getString("visit_time");
            arr.push_back(obj);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "Stats error: " << e.what() << std::endl;
        return "[]";
    }
    return arr.dump();
}

int Database::getUserIdFromSession(const std::string& session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT user_id FROM sessions WHERE session_id = ?"));
        pstmt->setString(1, session_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next())
            return res->getInt("user_id");
    } catch (sql::SQLException& e) {
        std::cerr << "Session check error: " << e.what() << std::endl;
    }
    return 0;
}