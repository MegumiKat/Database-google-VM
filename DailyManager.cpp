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

void DailyManager::createDailyData(double open, double high, double low, double close, double volume, const std::string& symbol) {
    try {
        work W(*conn);
        std::string sql = "INSERT INTO stock_daily_data (timestamp, open, high, low, close, volume, symbol) VALUES (" "NOW(), " + W.quote(open) + ", " + W.quote(high) + ", " + W.quote(low) + ", " + W.quote(close) + ", " + W.quote(volume) + ", " + W.quote(symbol) + ");";

        W.exec(sql);
        std::string checkSymbolSQL = "SELECT 1 FROM stock WHERE symbol = " + W.quote(symbol) + ";";
        pqxx::result symbolResult = W.exec(checkSymbolSQL);

        if (symbolResult.empty()) {
            // 如果symbol不存在，则插入新记录
            std::string insertStockSQL = "INSERT INTO stock (symbol, company_name, current_price) VALUES (" + W.quote(symbol) + ", " + W.quote(
                    symbol) + "," + W.quote(close) + ");";
            W.exec(insertStockSQL);
        } else {
            // 如果symbol存在，则更新current_price
            std::string updateStockSQL = "UPDATE stock SET current_price = " + W.quote(close) + " WHERE symbol = " + W.quote(symbol) + ";";
            W.exec(updateStockSQL);
        }

        W.commit();
        cout << "Daily data created successfully" << endl;
        cout << "_______________________________" << endl;
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

void DailyManager::getDailyData(int stockID, int num) {
    try {
        pqxx::nontransaction N(*conn);

        // 查询获取对应stock_id的symbol
        std::string getSymbolQuery = "SELECT symbol FROM stock WHERE stock_id = " + N.quote(std::to_string(stockID)) + ";";
        pqxx::result symbolResult(N.exec(getSymbolQuery));

        if (symbolResult.empty()) {
            std::cout << "No stock found with stock_id: " << stockID << std::endl;
            return;
        }

        std::string symbol = symbolResult[0]["symbol"].as<std::string>();

        // 查询stock_daily_data表中对应symbol的最近num条记录
        std::string sql = "SELECT data_id, timestamp, open, high, low, close, volume, symbol "
                          "FROM stock_daily_data "
                          "WHERE symbol = " + N.quote(symbol) + " "
                                                                "ORDER BY timestamp DESC "
                                                                "LIMIT " + N.quote(std::to_string(num)) + ";";

        pqxx::result R(N.exec(sql));

        if (R.empty()) {
            std::cout << "No daily data found for symbol: " << symbol << std::endl;
        } else {
            for (auto c = R.begin(); c != R.end(); ++c) {
                std::cout << "timestamp: " << c["timestamp"].as<std::string>() << std::endl;
                std::cout << "open: " << c["open"].as<double>() << std::endl;
                std::cout << "high: " << c["high"].as<double>() << std::endl;
                std::cout << "low: " << c["low"].as<double>() << std::endl;
                std::cout << "close: " << c["close"].as<double>() << std::endl;
                std::cout << "volume: " << c["volume"].as<int>() << std::endl;
                std::cout << "symbol: " << c["symbol"].as<std::string>() << std::endl;
                cout << "\n--------------------------" << endl;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error retrieving daily data: " << e.what() << std::endl;
    }
}