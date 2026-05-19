#pragma once
#include <string>

namespace sql {
    class Connection;
}

std::string random_digit_string_5();

std::string hash_password(const std::string& password);

bool verify_password(const std::string& password, const std::string& stored);

std::string generate_session_id();

std::string to_hex(const unsigned char* data, size_t len);