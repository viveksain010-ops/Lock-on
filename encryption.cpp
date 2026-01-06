#include "encryption.h"

std::string encrypt(std::string data, std::string key) {
    std::string result = data;
    for (size_t i = 0; i < data.length(); ++i) {
        result[i] ^= key[i % key.length()];
    }
    return result;
}

std::string decrypt(std::string data, std::string key) {
    return encrypt(data, key); // XOR is symmetric
}

