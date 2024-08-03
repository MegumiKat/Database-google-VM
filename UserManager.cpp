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
            //cout << "Opened database successfully: " << conn->dbname() << endl;
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
        string sql = "INSERT INTO users (user_name, password, email, name) VALUES (" +
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
        string sql = "DELETE FROM users WHERE user_id = " + W.quote(to_string(userID)) + ";";
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
        string sql = "UPDATE users SET user_name = " + W.quote(username) +
                     ", password = " + W.quote(password) +
                     ", email = " + W.quote(email) +
                     ", name = " + W.quote(name) +
                     " WHERE user_id = " + W.quote(to_string(userID)) + ";";
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
        string sql = "SELECT * FROM users WHERE user_id = " + N.quote(to_string(userID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No user found with user_id: " << userID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "user_name: " << c["user_name"].as<string>() << endl;
                cout << "password: " << c["password"].as<string>() << endl;
                cout << "email: " << c["email"].as<string>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving user: " << e.what() << std::endl;
    }
}
int UserManager::login(const string& username, const string& password) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM users WHERE user_name = " + N.quote(username) +
                     " AND password = " + N.quote(password) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "Login failed: Invalid username or password" << endl;
            return -1;
        } else {
            int userID = R[0]["user_id"].as<int>();
            cout << "Login successful for user: " << username << " with user_id: " << userID << endl;
            return userID;
        }
    } catch (const std::exception &e) {
        cerr << "Error during login: " << e.what() << std::endl;
        return -1;
    }
}