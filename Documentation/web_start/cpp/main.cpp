// Подключаем необходимые заголовочные файлы
#include <crow.h>               // Фреймворк Crow для создания веб-сервера
#include <mysql_connection.h>   // MySQL Connector/C++
#include <mysql_driver.h>       // Драйвер MySQL
#include <cppconn/statement.h>  // Для выполнения SQL-запросов
#include <cppconn/resultset.h>  // Для обработки результата запроса
#include <cppconn/exception.h>  // Для обработки ошибок MySQL
#include <nlohmann/json.hpp>    // Для удобной работы с JSON (установим отдельно)

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

    // 4. Запускаем сервер на порту 8080
    std::cout << "C++ сервер запущен на порту 8080" << std::endl;
    app.port(8080).multithreaded().run();

    // 5. Очистка ресурсов (при завершении работы)
    delete con;
    return 0;
}
