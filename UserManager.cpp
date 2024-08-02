//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "UserManager.h"

using namespace std;
using namespace pqxx;

UserManager::UserManager() {
    try {
        conn = new connection("dbname=mydb user=postgres password=123 hostaddr=127.0.0.1 port=5432");
        if (conn->is_open()) {
            cout << "Opened database successfully: " << conn->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

UserManager::~UserManager() {
    conn->disconnect();
    delete conn;
}

void UserManager::createUser(const string& username, const string& password, const string& email, const string& name) {
    try {
        work W(*conn);
        string sql = "INSERT INTO \"User\" (\"UserName\", \"Password\", \"Email\", \"Name\") VALUES (" +
                     W.quote(username) + ", " + W.quote(password) + ", " + W.quote(email) + ", " + W.quote(name) + ");";
        W.exec(sql);
        W.commit();
        cout << "User created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void UserManager::deleteUser(int userID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM \"User\" WHERE \"UserID\" = " + W.quote(userID) + ";";
        W.exec(sql);
        W.commit();
        cout << "User deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void UserManager::updateUser(int userID, const string& username, const string& password, const string& email, const string& name) {
    try {
        work W(*conn);
        string sql = "UPDATE \"User\" SET \"UserName\" = " + W.quote(username) +
                     ", \"Password\" = " + W.quote(password) +
                     ", \"Email\" = " + W.quote(email) +
                     ", \"Name\" = " + W.quote(name) +
                     " WHERE \"UserID\" = " + W.quote(to_string(userID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "User updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}
void UserManager::getUser(int userID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM \"User\" WHERE \"UserID\" = " + N.quote(userID) + ";";
        result R = N.exec(sql);
        cout << sql << endl;

        if (R.empty()) {
            cout << "No user found with UserID: " << userID << endl;
        } else {

            cout << "success" << endl;
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving user: " << e.what() << std::endl;
    }
}

