//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "ListManager.h"

using namespace std;
using namespace pqxx;

ListManager::ListManager() {
    try {
        conn = new connection("dbname=mydb user=postgres password=yourpassword hostaddr=127.0.0.1 port=5432");
        if (conn->is_open()) {
            //cout << "Opened database successfully: " << conn->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

ListManager::~ListManager() {
    conn->disconnect();
    delete conn;
}

void ListManager::createList(int userID, const string& name, bool isPublic) {
    try {
        work W(*conn);
        string sql = "INSERT INTO stock_list (user_id, name, public) VALUES (" +
                     W.quote(to_string(userID)) + ", " + W.quote(name) + ", " + W.quote(isPublic) + ");";
        W.exec(sql);
        W.commit();
        cout << "List created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void ListManager::deleteList(int listID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM stock_list WHERE list_id = " + W.quote(to_string(listID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "List deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void ListManager::updateList(int listID, const string& name, bool isPublic) {
    try {
        work W(*conn);
        string sql = "UPDATE stock_list SET name = " + W.quote(name) +
                     ", public = " + W.quote(isPublic) +
                     " WHERE list_id = " + W.quote(to_string(listID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "List updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void ListManager::getList(int listID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_list WHERE list_id = " + N.quote(to_string(listID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No list found with list_id: " << listID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
                cout << "public: " << (c["public"].as<bool>() ? "true" : "false") << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving list: " << e.what() << std::endl;
    }
}

bool ListManager::viewMyStockLists(int userID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_list WHERE user_id = " + N.quote(to_string(userID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stock lists found for user_id: " << userID << endl;
            return false;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
                cout << "public: " << c["public"].as<bool>() << endl;
            }
            return true;
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving stock lists: " << e.what() << std::endl;
        return false;
    }
}

bool ListManager::viewAllPublicStockLists() {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_list WHERE public = true;";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No public stock lists found." << endl;
            return false;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
                cout << "is_public: " << c["public"].as<bool>() << endl;
            }
            return true;
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving public stock lists: " << e.what() << std::endl;
        return false;
    }
}

void ListManager::shareStockListWithFriend(int listID, int friendUserID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT name, public FROM stock_list WHERE list_id = " + N.quote(to_string(listID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stock list found with list_id: " << listID << endl;
            return;
        }

        string name = R[0]["name"].as<string>();
        bool isPublic = R[0]["public"].as<bool>();

        work W(*conn);
        sql = "INSERT INTO stock_list (user_id, name, public) VALUES (" +
              W.quote(to_string(friendUserID)) + ", " + W.quote(name) + ", " + W.quote(isPublic) + ");";
        W.exec(sql);
        W.commit();
        cout << "Stock list shared with friend successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Error sharing stock list with friend: " << e.what() << std::endl;
    }
}