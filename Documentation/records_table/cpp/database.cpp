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

Database::Database(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
{
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    con.reset(driver->connect(host, user, pass));
    con->setSchema(db);
    std::cout << "Успешное подключение к MySQL!" << std::endl;
}

std::pair<int, std::string> Database::registerUser(const std::string& username, const std::string& password) {
    try {
        if (username.empty() || password.empty())
            return {400, R"({"message":"Username and password required"})"};

        std::string password_hash = hash_password(password);

        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO users (username, password_hash, verification_code) VALUES (?, ?, ?)"));
        pstmt->setString(1, username);
        pstmt->setString(2, password_hash);
        pstmt->setString(3, random_digit_string_5());
        pstmt->executeUpdate();
        return {201, R"({"message":"Registered"})"};
    } catch (sql::SQLException& e) {
        if (e.getErrorCode() == 1062) return {409, R"({"message":"Username exists"})"};
        std::cerr << "DB error in register: " << e.what() << std::endl;
        return {500, R"({"message":"DB error"})"};
    } catch (...) {
        return {500, R"({"message":"Server error"})"};
    }
}

std::pair<int, std::string> Database::loginUser(const std::string& username, const std::string& password, std::string& out_session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT id, password_hash FROM users WHERE username = ?"));
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (!res->next()) return {401, R"({"message":"Invalid username or password"})"};

        int user_id = res->getInt("id");
        std::string stored_hash = res->getString("password_hash");
        if (!verify_password(password, stored_hash)) return {401, R"({"message":"Invalid username or password"})"};

        out_session_id = generate_session_id();

        std::unique_ptr<sql::PreparedStatement> ins_sess(con->prepareStatement("INSERT INTO sessions (session_id, user_id, expires_at) VALUES (?, ?, DATE_ADD(NOW(), INTERVAL 1 DAY))"));
        ins_sess->setString(1, out_session_id);
        ins_sess->setInt(2, user_id);
        ins_sess->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> aboba(con->prepareStatement("UPDATE users SET verification_code = ? WHERE username = ?"));
        aboba->setString(2, username);
        aboba->setString(1, random_digit_string_5());
        aboba->executeUpdate();
        
        return {200, R"({"message":"Login successful"})"};
    } catch (sql::SQLException& e) {
        std::cerr << "Login error: " << e.what() << std::endl;
        return {500, R"({"message":"DB error"})"};
    } catch (...) {
        return {500, R"({"message":"Server error"})"};
    }
}

int Database::getUserIdFromSession(const std::string& session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT user_id FROM sessions WHERE session_id = ?"));
        pstmt->setString(1, session_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) return res->getInt("user_id");
    } catch (sql::SQLException& e) {
        std::cerr << "Session check error: " << e.what() << std::endl;
    }
    return 0;
}

std::string Database::getUserBySession(const std::string& session_id) {
    int user_id = getUserIdFromSession(session_id);
    if (user_id == 0) return "";
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT username FROM users WHERE id = ?"));
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) return res->getString("username");
    } catch (...) {}
    return "";
}

std::string Database::getVerificationCodeBySession(const std::string& session_id) {
    int user_id = getUserIdFromSession(session_id);
    if (user_id == 0) return "";
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT verification_code FROM users WHERE id = ?"));
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) return res->getString("verification_code");
    } catch (...) {}
    return "";
}

void Database::logout(const std::string& session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("DELETE FROM sessions WHERE session_id = ?"));
        pstmt->setString(1, session_id);
        pstmt->executeUpdate();
    } catch (...) {}
}

std::string Database::getLeaderboard(const std::string& game){
    nlohmann::json result;
    std::string table = game+"_records";

    nlohmann::json top_score = nlohmann::json::array();
    nlohmann::json top_time = nlohmann::json::array();
    nlohmann::json current_p;
    std::string score_query = 
            "SELECT u.username, r.score, r.created_at "
            "FROM " + table + " r "
            "JOIN users u ON r.user_id = u.id "
            "ORDER BY r.score DESC LIMIT 30";
    std::string time_query = 
            "SELECT u.username, r.time, r.created_at "
            "FROM " + table + " r "
            "JOIN users u ON r.user_id = u.id "
            "ORDER BY r.time DESC LIMIT 30";

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res_score(stmt->executeQuery(score_query));
        while (res_score->next()) {
            current_p["username"] = res_score->getString("username");
            current_p["score"] = res_score->getString("score");
            current_p["created_at"] = res_score->getString("created_at");
            top_score.push_back(current_p);
        }
        result["top_score"] = top_score;

        std::unique_ptr<sql::ResultSet> res_time(stmt->executeQuery(time_query));
        while (res_time->next()) {
            current_p["username"] = res_time->getString("username");
            current_p["time"] = res_time->getString("time");
            current_p["created_at"] = res_time->getString("created_at");
            top_time.push_back(current_p);
        }
        result["top_time"] = top_time;

    } catch (sql::SQLException& e) {
        std::cerr << "Liderboard error: " << e.what() << std::endl;
        return "[]";
    }
    return result.dump();
}