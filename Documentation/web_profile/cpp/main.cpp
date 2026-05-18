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
        
    // 4. Запускаем сервер на порту 8080
    std::cout << "C++ сервер запущен на порту 8080" << std::endl;
    
    app.port(8080).multithreaded().run();

    // 5. Очистка ресурсов (при завершении работы)
    delete con;
    return 0;
}
