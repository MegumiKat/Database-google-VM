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

void ListManager::deleteList(int listID, int userID) {
    try {
        nontransaction N(*conn);

        // 检查 listID 对应的 user_id 是否等于 userID
        string checkQuery = "SELECT user_id FROM stock_list WHERE list_id = " + N.quote(to_string(listID)) + ";";
        result checkResult(N.exec(checkQuery));

        if (checkResult.empty()) {
            cout << "List not found for listID: " << listID << endl;
            return;
        }

        int ownerID = checkResult[0]["user_id"].as<int>();
        if (ownerID != userID) {
            cout << "User is not authorized to delete this list." << endl;
            return;
        }

        // 结束非事务操作
        N.abort();

        // 开始事务操作删除 list 和相关的 review
        work W(*conn);


        // 删除 stock_list_holding 表中 list_id = listID 的数据
        string deleteHoldingsQuery = "DELETE FROM stock_list_holding WHERE list_id = " + W.quote(to_string(listID)) + ";";
        W.exec(deleteHoldingsQuery);

        string deleteReviewsQuery = "DELETE FROM review WHERE list_id = " + W.quote(to_string(listID)) + ";";
        W.exec(deleteReviewsQuery);

        string deleteListQuery = "DELETE FROM stock_list WHERE list_id = " + W.quote(to_string(listID)) + ";";
        W.exec(deleteListQuery);

        W.commit();
        cout << "List and associated reviews deleted successfully" << endl;

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
        string name;
        bool isPublic;

        // Start a non-transaction scope
        {
            nontransaction N(*conn);
            string sql = "SELECT name, public FROM stock_list WHERE list_id = " + N.quote(to_string(listID)) + ";";
            result R(N.exec(sql));

            if (R.empty()) {
                cout << "No stock list found with list_id: " << listID << endl;
                return;
            }

            name = R[0]["name"].as<string>();
            isPublic = R[0]["public"].as<bool>();
        } // nontransaction N ends here

        // Start a transaction to insert the new stock list
        work W(*conn);
        string sql = "INSERT INTO stock_list (user_id, name, public) VALUES (" +
                     W.quote(to_string(friendUserID)) + ", " + W.quote(name) + ", " + W.quote(isPublic) + ");";
        W.exec(sql);
        W.commit();
        cout << "Stock list shared with friend successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Error sharing stock list with friend: " << e.what() << std::endl;
    }
}


void ListManager::getAllStocks(int limit) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock ORDER BY stock_id LIMIT " + to_string(limit) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stocks found." << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                int stock_id = c["stock_id"].is_null() ? -1 : c["stock_id"].as<int>();
                string symbol = c["symbol"].is_null() ? "N/A" : c["symbol"].as<string>();
                string company_name = c["company_name"].is_null() ? "Unknown" : c["company_name"].as<string>();
                double current_price = c["current_price"].is_null() ? 0.0 : c["current_price"].as<double>();
                cout << "stock_id: " << stock_id << endl;
                cout << "symbol: " << symbol << endl;
                cout << "company_name: " << company_name << endl;
                cout << "current_price: " << current_price << endl;
                cout << "------------------------" << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving stocks: " << e.what() << std::endl;
    }
}