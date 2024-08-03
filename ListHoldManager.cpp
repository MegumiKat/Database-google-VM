//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "ListHoldManager.h"
#include <numeric>
#include <cmath>
#include <map>
using namespace std;
using namespace pqxx;

ListHoldManager::ListHoldManager() {
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

ListHoldManager::~ListHoldManager() {
    conn->disconnect();
    delete conn;
}

void ListHoldManager::createListHold(int listID, int stockID, int shares) {
    try {
        work W(*conn);
        string sql = "INSERT INTO stock_list_holding (list_id, stock_id, shares) VALUES (" +
                     W.quote(to_string(listID)) + ", " + W.quote(to_string(stockID)) + ", " + W.quote(to_string(shares)) + ");";
        W.exec(sql);
        W.commit();
        cout << "List hold created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void ListHoldManager::deleteListHold(int lHoldingID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM stock_list_holding WHERE l_holding_id = " + W.quote(to_string(lHoldingID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "List hold deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void ListHoldManager::updateListHold(int lHoldingID, int listID, int stockID, int shares) {
    try {
        work W(*conn);
        string sql = "UPDATE stock_list_holding SET list_id = " + W.quote(to_string(listID)) +
                     ", stock_id = " + W.quote(to_string(stockID)) +
                     ", shares = " + W.quote(to_string(shares)) +
                     " WHERE l_holding_id = " + W.quote(to_string(lHoldingID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "List hold updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void ListHoldManager::getListHold(int lHoldingID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_list_holding WHERE l_holding_id = " + N.quote(to_string(lHoldingID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No list hold found with l_holding_id: " << lHoldingID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "l_holding_id: " << c["l_holding_id"].as<int>() << endl;
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "stock_id: " << c["stock_id"].as<int>() << endl;
                cout << "shares: " << c["shares"].as<int>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving list hold: " << e.what() << std::endl;
    }
}


// 计算每日回报率
vector<double> ListHoldManager::calculateReturns(const vector<double>& prices) {
    vector<double> returns;
    for (size_t i = 1; i < prices.size(); ++i) {
        double ret = (prices[i] - prices[i-1]) / prices[i-1];
        returns.push_back(ret);
    }
    return returns;
}

// 计算平均值
double ListHoldManager::mean(const vector<double>& data) {
    return accumulate(data.begin(), data.end(), 0.0) / data.size();
}

// 计算标准差
double ListHoldManager::standardDeviation(const vector<double>& data, double mean) {
    double sum = 0.0;
    for (double value : data) {
        sum += pow(value - mean, 2);
    }
    return sqrt(sum / data.size());
}

// 计算协方差
double ListHoldManager::covariance(const vector<double>& data1, const vector<double>& data2, double mean1, double mean2) {
    double sum = 0.0;
    for (size_t i = 0; i < data1.size(); ++i) {
        sum += (data1[i] - mean1) * (data2[i] - mean2);
    }
    return sum / data1.size();
}

void ListHoldManager::viewStocksInList(int listID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT s.stock_id, s.symbol, s.company_name, s.current_price, l.shares "
                     "FROM stock s JOIN stock_list_holding l ON s.stock_id = l.stock_id "
                     "WHERE l.list_id = " + N.quote(to_string(listID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stocks found in list_id: " << listID << endl;
        } else {
            // 获取市场数据
            string marketQuery = "SELECT close FROM stock_daily_data ORDER BY timestamp;";
            result marketR(N.exec(marketQuery));
            vector<double> marketPrices;
            for (const auto& row : marketR) {
                marketPrices.push_back(row["close"].as<double>());
            }
            vector<double> marketReturns = calculateReturns(marketPrices);
            double marketMean = mean(marketReturns);
            double marketVariance = standardDeviation(marketReturns, marketMean);

            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
//                int stockID = c["stock_id"].as<int>();
//                string symbol = c["symbol"].as<string>();
//                string companyName = c["company_name"].as<string>();
//                double currentPrice = c["current_price"].as<double>();
//                int shares = c["shares"].as<int>();
                int stockID = c["stock_id"].is_null() ? 0 : c["stock_id"].as<int>();
                string symbol = c["symbol"].is_null() ? "Unknown" : c["symbol"].as<string>();
                string companyName = c["company_name"].is_null() ? "Unknown" : c["company_name"].as<string>();
                double currentPrice = c["current_price"].is_null() ? 0.0 : c["current_price"].as<double>();
                int shares = c["shares"].is_null() ? 0 : c["shares"].as<int>();

                // 获取股票的每日数据

                string stockQuery = "SELECT timestamp, open, high, low, close, volume FROM stock_daily_data WHERE symbol = " + N.quote(symbol) + " ORDER BY timestamp DESC LIMIT 3;";
//                cout << "stockQuery: "<< stockQuery <<endl;
                result stockR(N.exec(stockQuery));
                vector<double> stockPrices;
                for (const auto& row : stockR) {
                    stockPrices.push_back(row["close"].as<double>());
                    cout << "Timestamp: " << row["timestamp"] << ", Open: " << row["open"].as<double>()
                         << ", High: " << row["high"].as<double>() << ", Low: " << row["low"].as<double>()
                         << ", Close: " << row["close"].as<double>() << ", Volume: " << row["volume"].as<double>() << endl;
                }
                vector<double> stockReturns = calculateReturns(stockPrices);
                double stockMean = mean(stockReturns);
                double stockStdDev = standardDeviation(stockReturns, stockMean);
                double cv = stockStdDev / stockMean;
                double cov = covariance(stockReturns, marketReturns, stockMean, marketMean);
                double beta = cov / marketVariance;

                cout << "Stock ID: " << stockID << ", Symbol: " << symbol << ", Company Name: " << companyName
                     << ", Current Price: " << currentPrice << ", Shares: " << shares << endl;
                cout << "Coefficient of Variation (CV): " << cv << ", Beta: " << beta << endl;
                cout << "----------------------------" << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving stocks in list: " << e.what() << std::endl;
    }
}


void ListHoldManager::calculateCovarianceMatrix(int listID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT s.stock_id, s.symbol, l.shares "
                     "FROM stock s JOIN stock_list_holding l ON s.stock_id = l.stock_id "
                     "WHERE l.list_id = " + N.quote(to_string(listID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stocks found in list_id: " << listID << endl;
            return;
        }

        // 存储每只股票的每日收盘价和每日回报率
        unordered_map<int, vector<double>> stockPrices;
        unordered_map<int, vector<double>> stockReturns;
        vector<string> symbols;

        for (const auto& row : R) {
            int stockID = row["stock_id"].is_null() ? 0 : row["stock_id"].as<int>();
            string symbol = row["symbol"].is_null() ? "Unknown" : row["symbol"].as<string>();
            symbols.push_back(symbol);

            // 获取股票的每日数据
            string stockQuery = "SELECT close FROM stock_daily_data WHERE symbol = '" + to_string(symbol) + "' ORDER BY timestamp;";
            result stockR(N.exec(stockQuery));
            vector<double> prices;
            for (const auto& priceRow : stockR) {
                prices.push_back(priceRow["close"].as<double>());
            }

            stockPrices[stockID] = prices;
            stockReturns[stockID] = calculateReturns(prices);
        }

        // 计算协方差矩阵
        size_t n = symbols.size();
        vector<vector<double>> covarianceMatrix(n, vector<double>(n, 0.0));

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = i; j < n; ++j) {
                double mean1 = mean(stockReturns[i]);
                double mean2 = mean(stockReturns[j]);
                double cov = covariance(stockReturns[i], stockReturns[j], mean1, mean2);
                covarianceMatrix[i][j] = cov;
                covarianceMatrix[j][i] = cov;
            }
        }

        // 打印协方差矩阵
        cout << "Covariance Matrix for Stock List ID: " << listID << endl;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                cout << covarianceMatrix[i][j] << " ";
            }
            cout << endl;
        }

    } catch (const std::exception &e) {
        cerr << "Error calculating covariance matrix: " << e.what() << std::endl;
    }
}