#include "User.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

User::User(const std::string& uname, const std::string& pwd)
    : username(uname), password(pwd) {
}

bool User::isUsernameTaken(const std::string& username) {
    std::ifstream file("users.txt");
    std::string line, uname;
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> uname;
        if (uname == username) return true;
    }
    return false;
}

bool User::saveUserCredentials(const std::string& username, const std::string& password) {
    std::ofstream file("users.txt", std::ios::app);
    if (!file) return false;
    file << username << " " << password << "\n";
    return true;
}

bool User::deleteUserFromFile(const std::string& username) {
    std::ifstream file("users.txt");
    std::ostringstream oss;
    std::string line, uname;
    bool found = false;
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> uname;
        if (uname != username) oss << line << "\n";
        else found = true;
    }
    file.close();
    std::ofstream out("users.txt");
    out << oss.str();
    return found;
}

bool User::deleteUserFolder(const std::string& username) {
    std::error_code ec;
    std::filesystem::remove_all("data/users/" + username, ec);
    return !ec;
}

bool User::renameUser(const std::string& oldUsername, const std::string& newUsername) {
    if (isUsernameTaken(newUsername)) return false;
    std::ifstream file("users.txt");
    std::ostringstream oss;
    std::string line, uname, pwd;
    bool found = false;
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> uname >> pwd;
        if (uname == oldUsername) {
            oss << newUsername << " " << pwd << "\n";
            found = true;
        }
        else {
            oss << line << "\n";
        }
    }
    file.close();
    std::ofstream out("users.txt");
    out << oss.str();
    // Rename folder
    std::error_code ec;
    std::filesystem::rename("data/users/" + oldUsername, "data/users/" + newUsername, ec);
    return found && !ec;
}

std::map<std::string, std::string> User::loadAllUsers() {
    std::ifstream file("users.txt");
    std::map<std::string, std::string> users;
    std::string line, uname, pwd;
    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> uname >> pwd;
        users[uname] = pwd;
    }
    return users;
}

std::string User::loadUserData(const std::string& filename) const {
    std::ifstream file("data/users/" + username + "/" + filename);
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

void User::saveUserData(const std::string& filename, const std::string& data) const {
    std::ofstream file("data/users/" + username + "/" + filename);
    file << data;
}

std::string User::getUsername() const {
    return username;
}