#include "functions.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstdlib>
#include <vector>
#include <stdexcept>

std::string random_digit_string_5() {
    const int length = 5;
    std::string result;
    result.reserve(length);
    unsigned char byte;
    for (int i = 0; i < length; ++i) {
        while (true) {
            if (RAND_bytes(&byte, 1) != 1) {
                throw std::runtime_error("RAND_bytes failed in random_digit_string_5");
            }
            if (byte < 250) {
                result.push_back('0' + (byte % 10));
                break;
            }
        }
    }
    return result;
}

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

std::string generate_session_id() {
    unsigned char buf[32];
    if (RAND_bytes(buf, sizeof(buf)) != 1) {
        throw std::runtime_error("RAND_bytes failed");
    }
    return to_hex(buf, sizeof(buf));
}

std::string hash_password(const std::string& password) {
    unsigned char salt[32];
    if (RAND_bytes(salt, sizeof(salt)) != 1)
        throw std::runtime_error("RNG error");
    unsigned char hash[32];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(), salt, sizeof(salt), 10000, EVP_sha256(), sizeof(hash), hash) != 1)
        throw std::runtime_error("Hash error");
    return to_hex(salt, sizeof(salt)) + "$" + to_hex(hash, sizeof(hash));
}

bool verify_password(const std::string& password, const std::string& stored) {
    auto pos = stored.find('$');
    if (pos == std::string::npos) return false;

    std::string salt_hex = stored.substr(0, pos);
    std::string hash_hex_expected = stored.substr(pos + 1);

    std::vector<unsigned char> salt;
    for (size_t i = 0; i < salt_hex.length(); i += 2) {
        std::string byteStr = salt_hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteStr.c_str(), nullptr, 16);
        salt.push_back(byte);
    }

    unsigned char hash[32];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                          salt.data(), salt.size(), 10000,
                          EVP_sha256(), sizeof(hash), hash) != 1)
        return false;

    return to_hex(hash, sizeof(hash)) == hash_hex_expected;
}