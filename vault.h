#ifndef VAULT_H
#define VAULT_H

#include <string>

bool authenticate(std::string masterPassword);
void addPassword(std::string key);
void viewPasswords(std::string key);
void searchPassword(std::string key);
void deletePassword(std::string key);

#endif

