// vault.cpp  (macOS-friendly)
#include "vault.h"
#include "encryption.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>
#include <sys/stat.h>   // chmod
#include <termios.h>    // hidden input
#include <unistd.h>     // access(), getuid()
#include <pwd.h>        // getpwuid()

using namespace std;
namespace fs = std::filesystem;

// Helper: get user's home directory
static string getHomeDir() {
    if (const char* home = getenv("HOME")) return string(home);
    struct passwd* pw = getpwuid(getuid());
    if (pw) return string(pw->pw_dir);
    return string(); // fallback empty
}

// Data file path: ~/Library/Application Support/VaultX/data.txt
static string getDataFilePath() {
    string home = getHomeDir();
    if (home.empty()) home = "."; // fallback
    string dir = home + "/Library/Application Support/VaultX";
    fs::create_directories(dir); // safe if exists
    return dir + "/data.txt";
}

// Set file permission to 600 (rw-------) for privacy
static void setFilePermissions600(const string& path) {
    // chmod expects mode_t
    chmod(path.c_str(), S_IRUSR | S_IWUSR);
}

// Read hidden input (password) from terminal
static string getHiddenInput(const string& prompt) {
    cout << prompt;
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    string input;
    std::getline(cin, input);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << '\n';
    return input;
}

// Simple authentication check (for demo). You can replace with hashed check later.
bool authenticate(const string& masterPassword) {
    return masterPassword == "admin123";
}

// Add a password entry (uses full-line inputs, supports spaces)
void addPassword(string key) {
    string website, username, password;
    cout << "Website: ";        std::getline(cin, website);
    cout << "Username: ";       std::getline(cin, username);
    cout << "Password: ";       std::getline(cin, password);

    string line = website + "," + username + "," + password;

    string dataPath = getDataFilePath();
    ofstream file(dataPath, ios::app);
    if (!file) {
        cerr << "Failed to open data file for writing: " << dataPath << '\n';
        return;
    }
    file << encrypt(line, key) << '\n';
    file.close();

    // Ensure correct (private) permissions
    setFilePermissions600(dataPath);

    cout << "Password added successfully.\n";
}

void viewPasswords(string key) {
    string dataPath = getDataFilePath();
    ifstream file(dataPath);
    if (!file) {
        cout << "No stored credentials found.\n";
        return;
    }

    string line;
    cout << "\n--- Stored Credentials ---\n";
    while (getline(file, line)) {
        if (line.empty()) continue;
        string decrypted = decrypt(line, key);
        replace(decrypted.begin(), decrypted.end(), ',', '\t');
        cout << decrypted << '\n';
    }
    file.close();
}

void searchPassword(string key) {
    cout << "Enter website to search: ";
    string target;
    std::getline(cin, target);

    string dataPath = getDataFilePath();
    ifstream file(dataPath);
    if (!file) {
        cout << "No stored credentials found.\n";
        return;
    }

    string line;
    bool found = false;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string decrypted = decrypt(line, key);
        if (decrypted.find(target) != string::npos) {
            replace(decrypted.begin(), decrypted.end(), ',', '\t');
            cout << decrypted << '\n';
            found = true;
        }
    }
    if (!found) cout << "No match found.\n";
    file.close();
}

void deletePassword(string key) {
    cout << "Enter website to delete: ";
    string target;
    std::getline(cin, target);

    string dataPath = getDataFilePath();
    ifstream file(dataPath);
    if (!file) {
        cout << "No stored credentials found.\n";
        return;
    }

    string tempPath = dataPath + ".tmp";
    ofstream temp(tempPath);
    if (!temp) {
        cerr << "Failed to open temp file for writing.\n";
        file.close();
        return;
    }

    string line;
    bool deleted = false;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string decrypted = decrypt(line, key);
        if (decrypted.find(target) == string::npos) {
            temp << line << '\n';
        } else {
            deleted = true;
        }
    }

    file.close();
    temp.close();

    // Replace original file
    fs::rename(tempPath, dataPath);

    // Ensure permissions
    setFilePermissions600(dataPath);

    if (deleted)
        cout << "Entry deleted.\n";
    else
        cout << "Website not found.\n";
}
