//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "DailyManager.h"

using namespace std;
using namespace pqxx;

DailyManager::DailyManager() {
    try {
        conn = new connection("dbname=mydb user=postgres password=yourpassword hostaddr=127.0.0.1 port=5432");
        if (conn->is_open()) {
            // cout << "Opened database successfully: " << conn->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

DailyManager::~DailyManager() {
    conn->disconnect();
    delete conn;
}

void DailyManager::createDailyData(int stockID, const string& timestamp, double open, double high, double low, double close, int volume) {
    try {
        work W(*conn);
        string sql = "INSERT INTO stock_daily_data (stock_id, timestamp, open, high, low, close, volume) VALUES (" +
                     W.quote(to_string(stockID)) + ", " + W.quote(timestamp) + ", " + W.quote(to_string(open)) + ", " +
                     W.quote(to_string(high)) + ", " + W.quote(to_string(low)) + ", " + W.quote(to_string(close)) + ", " + W.quote(to_string(volume)) + ");";
        W.exec(sql);
        W.commit();
        cout << "Daily data created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void DailyManager::deleteDailyData(int dataID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM stock_daily_data WHERE data_id = " + W.quote(to_string(dataID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Daily data deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void DailyManager::updateDailyData(int dataID, int stockID, const string& timestamp, double open, double high, double low, double close, int volume) {
    try {
        work W(*conn);
        string sql = "UPDATE stock_daily_data SET stock_id = " + W.quote(to_string(stockID)) +
                     ", timestamp = " + W.quote(timestamp) +
                     ", open = " + W.quote(to_string(open)) +
                     ", high = " + W.quote(to_string(high)) +
                     ", low = " + W.quote(to_string(low)) +
                     ", close = " + W.quote(to_string(close)) +
                     ", volume = " + W.quote(to_string(volume)) +
                     " WHERE data_id = " + W.quote(to_string(dataID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Daily data updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void DailyManager::getDailyData(int dataID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_daily_data WHERE data_id = " + N.quote(to_string(dataID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No daily data found with data_id: " << dataID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "data_id: " << c["data_id"].as<int>() << endl;
                cout << "stock_id: " << c["stock_id"].as<int>() << endl;
                cout << "timestamp: " << c["timestamp"].as<string>() << endl;
                cout << "open: " << c["open"].as<double>() << endl;
                cout << "high: " << c["high"].as<double>() << endl;
                cout << "low: " << c["low"].as<double>() << endl;
                cout << "close: " << c["close"].as<double>() << endl;
                cout << "volume: " << c["volume"].as<int>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving daily data: " << e.what() << std::endl;
    }
}

