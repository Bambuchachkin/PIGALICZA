#pragma once
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

// Предварительные объявления
namespace sql {
    class Connection;
}

class Database {
public:
    // Конструктор подключается к БД
    Database(const std::string& host, const std::string& user,
             const std::string& pass, const std::string& db);
    ~Database();

    // Запрещаем копирование
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Регистрация нового пользователя
    // Возвращает JSON-ответ (message, код ошибки зашит в строку, 
    // но для простоты возвращаем структуру)
    // Можно возвращать пару: HTTP-статус и JSON-строку.
    std::pair<int, std::string> registerUser(const std::string& username, const std::string& password);

    // Вход: возвращает статус и либо ошибку, либо session_id
    std::pair<int, std::string> loginUser(const std::string& username, const std::string& password,
                                          std::string& out_session_id);

    // Получить пользователя по session_id (возвращает username или пустую строку)
    std::string getUserBySession(const std::string& session_id);

    // Выход (удаление сессии)
    void logout(const std::string& session_id);

    // Статистика (последние 10 визитов)
    std::string getStats();

    // Получить user_id по сессии (для внутренних нужд)
    int getUserIdFromSession(const std::string& session_id);

    // Доступ к сырому соединению (если понадобится)
    sql::Connection* getConnection() { return con.get(); }

private:
    std::unique_ptr<sql::Connection> con;
};