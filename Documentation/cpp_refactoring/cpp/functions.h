#pragma once
#include <string>

namespace sql {
    class Connection;   // forward declaration
}

// Хеширует пароль и возвращает строку "salt_hex$hash_hex"
std::string hash_password(const std::string& password);

// Проверяет пароль по сохранённой строке
bool verify_password(const std::string& password, const std::string& stored);

// Генерирует случайный идентификатор сессии
std::string generate_session_id();

// Преобразует массив байт в шестнадцатеричную строку
std::string to_hex(const unsigned char* data, size_t len);