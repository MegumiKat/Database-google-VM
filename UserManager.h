//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_USERMANAGER_H
#define DATABASE_GOOGLE_VM_USERMANAGER_H


#include <iostream>
#include <pqxx/pqxx>

class UserManager {
public:
    UserManager();
    ~UserManager();
    void createUser(const std::string& username, const std::string& password, const std::string& email, const std::string& name);
    void deleteUser(int userID);
    void updateUser(int userID, const std::string& username, const std::string& password, const std::string& email, const std::string& name);
    void getUser(int userID);

private:
    pqxx::connection *conn;
};

#endif //DATABASE_GOOGLE_VM_USERMANAGER_H
