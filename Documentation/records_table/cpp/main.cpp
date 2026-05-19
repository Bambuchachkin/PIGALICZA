#include <crow.h>
#include "database.h"
#include <string>

int main() {
    Database db("tcp://127.0.0.1:3306", "my_user", "aboba123", "pigalicza_db");
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/register").methods(crow::HTTPMethod::POST)([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        auto [status, answer] = db.registerUser(body["username"].s(), body["password"].s());
        crow::response resp(status, answer);
        resp.set_header("Content-Type", "application/json");
        return resp;
    });

    CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::POST)([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        std::string session_id;
        auto [status, answer] = db.loginUser(body["username"].s(), body["password"].s(), session_id);
        crow::response resp(status, answer);
        resp.set_header("Content-Type", "application/json");
        if (status == 200) {
            resp.set_header("Set-Cookie", "session_id=" + session_id + "; Path=/; HttpOnly; Max-Age=86400; SameSite=Lax");
        }
        return resp;
    });

    CROW_ROUTE(app, "/api/me").methods(crow::HTTPMethod::GET)([&db](const crow::request& req) {
        std::string cookie = req.get_header_value("Cookie");
        std::string session_id;
        if (!cookie.empty()) {
            size_t pos = cookie.find("session_id=");
            if (pos != std::string::npos) {
                size_t start = pos + 11;
                size_t end = cookie.find(';', start);
                session_id = cookie.substr(start, end - start);
            }
        }
        if (session_id.empty()) return crow::response(401, R"({"message":"Not authenticated"})");
        std::string username = db.getUserBySession(session_id);
        if (username.empty()) return crow::response(401, R"({"message":"Session invalid or expired"})");
        std::string verification_code = db.getVerificationCodeBySession(session_id);
        if (verification_code.empty()) return crow::response(401, R"({"message":"Session invalid or expired"})");

        nlohmann::json result;
        result["username"] = username;
        result["verification_code"] = verification_code;
        return crow::response(200, result.dump());
    });

    CROW_ROUTE(app, "/api/logout").methods(crow::HTTPMethod::POST)([&db](const crow::request& req) {
        std::string cookie = req.get_header_value("Cookie");
        std::string session_id;
        if (!cookie.empty()){
            size_t pos = cookie.find("session_id=");
            if (pos != std::string::npos){
                size_t start = pos + 11;
                size_t end = cookie.find(';', start);
                session_id = cookie.substr(start, end - start);
            }
        }
        db.logout(session_id);
        crow::response resp(200, R"({"message":"Logged out"})");
        resp.set_header("Content-Type", "application/json");
        resp.set_header("Set-Cookie", "session_id=; Path=/; HttpOnly; Max-Age=0");
        return resp;
    });

    CROW_ROUTE(app, "/api/leaderboard/<string>").methods(crow::HTTPMethod::GET)([&db](const crow::request& req, std::string game) {
        std::string json = db.getLeaderboard(game);
        crow::response resp(200, json);
        resp.set_header("Content-Type", "application/json");
        return resp;
    });

    std::cout << "C++ сервер запущен на порту 8080" << std::endl;
    app.port(8080).multithreaded().run();
}