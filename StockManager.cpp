//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "StockManager.h"
#include "DailyManager.h"

using namespace std;
using namespace pqxx;

StockManager::StockManager() {
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

StockManager::~StockManager() {
    conn->disconnect();
    delete conn;
}

void StockManager::createStock(const string& symbol, const string& companyName) {
    try {
        // Check if the stock with the same symbol already exists
        nontransaction N(*conn);
        string checkSql = "SELECT * FROM stock WHERE symbol = " + N.quote(symbol) + ";";
        result R(N.exec(checkSql));

        if (!R.empty()) {
            cout << "Stock with symbol " << symbol << " already exists." << endl;
            return;
        }

        // If the stock doesn't exist, insert the new stock
        work W(*conn);
        string sql = "INSERT INTO stock (symbol, company_name) VALUES (" +
                     W.quote(symbol) + ", " + W.quote(companyName) + ");";
        W.exec(sql);
        W.commit();
        cout << "Stock created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}


void StockManager::deleteStock(int stockID) {
    try {
        work W(*conn);
        string sql1 = "DELETE FROM stock_daily_data WHERE stock_id = " + W.quote(to_string(stockID)) + ";";
        string sql = "DELETE FROM stock WHERE stock_id = " + W.quote(to_string(stockID)) + ";";
        W.exec(sql1);
        W.exec(sql);
        W.commit();
        cout << "Stock deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void StockManager::updateStock(int stockID, const string& symbol, const string& companyName) {
    try {
        work W(*conn);
        string sql = "UPDATE stock SET symbol = " + W.quote(symbol) +
                     ", company_name = " + W.quote(companyName) +
                     " WHERE stock_id = " + W.quote(to_string(stockID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Stock updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void StockManager::getStock(int stockID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock WHERE stock_id = " + N.quote(to_string(stockID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stock found with stock_id: " << stockID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "stock_id: " << c["stock_id"].as<int>() << endl;
                cout << "symbol: " << c["symbol"].as<string>() << endl;
                cout << "company_name: " << c["company_name"].as<string>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving stock: " << e.what() << std::endl;
    }
}
