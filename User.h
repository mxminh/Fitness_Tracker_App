#ifndef USER_H
#define USER_H

#include <string>
#include <map>

class User {
private:
    std::string username;
    std::string password;

public:
    User(const std::string& uname, const std::string& pwd);
    static bool isUsernameTaken(const std::string& username);
    static bool saveUserCredentials(const std::string& username, const std::string& password);
    static bool deleteUserFromFile(const std::string& username);
    static bool deleteUserFolder(const std::string& username);
    static bool renameUser(const std::string& oldUsername, const std::string& newUsername);
    static std::map<std::string, std::string> loadAllUsers();

    std::string loadUserData(const std::string& filename) const;
    void saveUserData(const std::string& filename, const std::string& data) const;
    std::string getUsername() const;
};

#endif