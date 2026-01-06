#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

std::string encrypt(std::string data, std::string key);
std::string decrypt(std::string data, std::string key);

#endif

