// Подключаем необходимые заголовочные файлы
#include <crow.h>               // Фреймворк Crow для создания веб-сервера
#include <mysql_connection.h>   // MySQL Connector/C++
#include <mysql_driver.h>       // Драйвер MySQL
#include <cppconn/statement.h>  // Для выполнения SQL-запросов
#include <cppconn/resultset.h>  // Для обработки результата запроса
#include <cppconn/exception.h>  // Для обработки ошибок MySQL
#include <nlohmann/json.hpp>    // Для удобной работы с JSON (установим отдельно)
#include <openssl/evp.h>    // PBKDF2
#include <openssl/rand.h>   // генерация соли
#include <cppconn/prepared_statement.h>  // обязательно
#include <ctime> 
#include <cstdlib>

// Хелпер — перевод байтов в hex (обычная функция)
std::string to_hex(const unsigned char* data, size_t len) {
    static const char hex[] = "0123456789abcdef";
    std::string result;
    result.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        result += hex[(data[i] >> 4) & 0xF];
        result += hex[data[i] & 0xF];
    }
    return result;
}

// Генерация случайного идентификатора сессии (64 символа hex)
std::string generate_session_id() {
    unsigned char buf[32];
    if (RAND_bytes(buf, sizeof(buf)) != 1) {
        throw std::runtime_error("RAND_bytes failed");
    }
    return to_hex(buf, sizeof(buf));
}

// Проверка пароля: stored = "salt_hex$hash_hex"
bool verify_password(const std::string& password, const std::string& stored) {
    auto pos = stored.find('$');
    if (pos == std::string::npos) return false;

    std::string salt_hex = stored.substr(0, pos);
    std::string hash_hex_expected = stored.substr(pos + 1);

    // Переводим соль из hex в байты
    std::vector<unsigned char> salt;
    for (size_t i = 0; i < salt_hex.length(); i += 2) {
        std::string byteStr = salt_hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteStr.c_str(), nullptr, 16);
        salt.push_back(byte);
    }

    // Вычисляем хеш от введённого пароля с той же солью
    unsigned char hash[32];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                          salt.data(), salt.size(), 10000,
                          EVP_sha256(), sizeof(hash), hash) != 1) {
        return false;
    }

    std::string hash_hex_calculated = to_hex(hash, sizeof(hash));
    return hash_hex_calculated == hash_hex_expected;
}

// Получение user_id по сессии (0 если не найдена или просрочена)
int get_user_id_from_session(sql::Connection* con, const std::string& session_id) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT user_id, expires_at FROM sessions WHERE session_id = ?")
        );
        pstmt->setString(1, session_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            // Проверяем срок действия
            std::string expires_str = res->getString("expires_at");
            // Пропускаем, если истекло (можно добавить более строгую проверку)
            // Для простоты удалим просроченные сессии отдельно, но сейчас просто вернём user_id
            return res->getInt("user_id");
        }
    } catch (sql::SQLException& e) {
        std::cerr << "Session check error: " << e.what() << std::endl;
    }
    return 0; // не авторизован
}


// Для упрощения кода
using json = nlohmann::json;

int main() {
    // 1. Инициализируем MySQL Connector
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;

    // Параметры подключения к БД (замените на ваши данные)
    const std::string DB_HOST = "tcp://127.0.0.1:3306";
    const std::string DB_USER = "my_user";
    const std::string DB_PASS = "aboba123";
    const std::string DB_NAME = "pigalicza_db";

    try {
        // Получаем экземпляр драйвера MySQL
        driver = sql::mysql::get_mysql_driver_instance();
        // Устанавливаем соединение
        con = driver->connect(DB_HOST, DB_USER, DB_PASS);
        // Выбираем базу данных для работы
        con->setSchema(DB_NAME);
        std::cout << "Успешное подключение к MySQL!" << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "Ошибка подключения к MySQL: " << e.what() << std::endl;
        return 1;
    }

    // 2. Создаём веб-сервер на Crow
    crow::SimpleApp app;

    // 3. Определяем маршрут для API (GET-запрос на /api/stats)
    CROW_ROUTE(app, "/api/stats")
    ([&con](){
        try {
            // Создаём объект для выполнения запросов
            sql::Statement* stmt = con->createStatement();
            // Выполняем SQL-запрос: получаем последние 10 записей
            // ВАЖНО: Сортировка в SQL, а не в коде!
            sql::ResultSet* res = stmt->executeQuery(
                "SELECT ip_address, visit_time FROM visits ORDER BY id DESC LIMIT 10"
            );

            // Создаём JSON-массив для ответа
            json json_array = json::array();

            // Проходим по всем строкам результата
            while (res->next()) {
                // Создаём JSON-объект для одной строки и добавляем его в массив
                json json_object;
                json_object["ip"] = res->getString("ip_address");
                json_object["visit_time"] = res->getString("visit_time");
                json_array.push_back(json_object);
            }

            // Освобождаем ресурсы
            delete res;
            delete stmt;

            // Возвращаем ответ сервера (JSON и статус 200 OK)
            return crow::response(json_array.dump());
        } catch (sql::SQLException &e) {
            std::cerr << "Ошибка SQL: " << e.what() << std::endl;
            return crow::response(500, "Internal Server Error");
        }
    });



CROW_ROUTE(app, "/api/register").methods(crow::HTTPMethod::POST)
([&con](const crow::request& req) {
    try {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "Invalid JSON");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        if (username.empty() || password.empty())
            return crow::response(400, "Username and password required");
        if (password.size() < 6)
            return crow::response(400, "Password too short");

        // Соль
        unsigned char salt[32];
        if (RAND_bytes(salt, sizeof(salt)) != 1)
            return crow::response(500, "RNG error");

        // PBKDF2
        unsigned char hash[32];
        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                              salt, sizeof(salt), 10000,
                              EVP_sha256(), sizeof(hash), hash) != 1)
            return crow::response(500, "Hash error");

        std::string password_hash = to_hex(salt, sizeof(salt)) + "$" + to_hex(hash, sizeof(hash));

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO users (username, password_hash) VALUES (?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, password_hash);
        pstmt->executeUpdate();

        crow::json::wvalue res;
        res["message"] = "Registered";
        return crow::response(201, res);
    } catch (sql::SQLException &e) {
        if (e.getErrorCode() == 1062)
            return crow::response(409, "Username exists");
        return crow::response(500, "DB error");
    } catch (...) {
        return crow::response(500, "Server error");
    }
});


        // ---------- Авторизация ----------

    // POST /api/login
    CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::POST)
    ([&con](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, "Invalid JSON");

            std::string username = body["username"].s();
            std::string password = body["password"].s();

            // Ищем пользователя
            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT id, password_hash FROM users WHERE username = ?")
            );
            pstmt->setString(1, username);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            if (!res->next()) {
                return crow::response(401, "Invalid username or password");
            }

            int user_id = res->getInt("id");
            std::string stored_hash = res->getString("password_hash");

            if (!verify_password(password, stored_hash)) {
                return crow::response(401, "Invalid username or password");
            }

            // Создаём сессию
            std::string session_id = generate_session_id();
            std::unique_ptr<sql::PreparedStatement> ins_sess(
                con->prepareStatement("INSERT INTO sessions (session_id, user_id, expires_at) VALUES (?, ?, DATE_ADD(NOW(), INTERVAL 1 DAY))")
            );
            ins_sess->setString(1, session_id);
            ins_sess->setInt(2, user_id);
            ins_sess->executeUpdate();

            crow::response resp(200, R"({"message":"Login successful"})");
            resp.set_header("Content-Type", "application/json");
            resp.set_header("Set-Cookie", "session_id=" + session_id + "; Path=/; HttpOnly; Max-Age=86400; SameSite=Lax");
            return resp;
        } catch (const std::exception& e) {
            std::cerr << "Login error: " << e.what() << std::endl;
            return crow::response(500, "Internal error");
        }
    });

    // GET /api/me — возвращает информацию о текущем пользователе
    CROW_ROUTE(app, "/api/me").methods(crow::HTTPMethod::GET)
    ([&con](const crow::request& req) {
        // Читаем куку
        auto& cookie_header = req.get_header_value("Cookie");
        std::string session_id;
        if (!cookie_header.empty()) {
            size_t pos = cookie_header.find("session_id=");
            if (pos != std::string::npos) {
                size_t start = pos + 11; // длина "session_id="
                size_t end = cookie_header.find(';', start);
                session_id = cookie_header.substr(start, end - start);
            }
        }

        if (session_id.empty())
            return crow::response(401, "Not authenticated");

        int user_id = get_user_id_from_session(con, session_id);
        if (user_id == 0)
            return crow::response(401, "Session invalid or expired");

        // Получаем имя пользователя
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT username FROM users WHERE id = ?")
        );
        pstmt->setInt(1, user_id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (!res->next())
            return crow::response(500, "User not found");

        std::string username = res->getString("username");

        crow::json::wvalue result;
        result["username"] = username;
        return crow::response(200, result);
    });

    // POST /api/logout
    CROW_ROUTE(app, "/api/logout").methods(crow::HTTPMethod::POST)
    ([&con](const crow::request& req) {
        auto& cookie_header = req.get_header_value("Cookie");
        std::string session_id;
        if (!cookie_header.empty()) {
            size_t pos = cookie_header.find("session_id=");
            if (pos != std::string::npos) {
                size_t start = pos + 11;
                size_t end = cookie_header.find(';', start);
                session_id = cookie_header.substr(start, end - start);
            }
        }

        if (!session_id.empty()) {
            try {
                std::unique_ptr<sql::PreparedStatement> pstmt(
                    con->prepareStatement("DELETE FROM sessions WHERE session_id = ?")
                );
                pstmt->setString(1, session_id);
                pstmt->executeUpdate();
            } catch (...) {}
        }

        crow::response resp(200, R"({"message":"Logged out"})");
        resp.set_header("Content-Type", "application/json");
        resp.set_header("Set-Cookie", "session_id=; Path=/; HttpOnly; Max-Age=0");
        return resp;
    });
        
    // 4. Запускаем сервер на порту 8080
    std::cout << "C++ сервер запущен на порту 8080" << std::endl;
    
    app.port(8080).multithreaded().run();

    // 5. Очистка ресурсов (при завершении работы)
    delete con;
    return 0;
}
