#include "PortfolioManager.h"
#include <numeric>
#include <cmath>
#include <map>

using namespace std;
using namespace pqxx;

PortfolioManager::PortfolioManager() {
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

PortfolioManager::~PortfolioManager() {
    conn->disconnect();
    delete conn;
}

void PortfolioManager::createPortfolio(int userID, const string& name, double cashBalance) {
    try {
        work W(*conn);
        string sql = "INSERT INTO portfolio (user_id, name, cash_balance) VALUES (" +
                     W.quote(to_string(userID)) + ", " + W.quote(name) + ", " + W.quote(to_string(cashBalance)) + ");";
        W.exec(sql);
        W.commit();
        cout << "Portfolio created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void PortfolioManager::deletePortfolio(int portfolioID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM portfolio WHERE portfolio_id = " + W.quote(to_string(portfolioID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Portfolio deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void PortfolioManager::updatePortfolio(int portfolioID, const string& name, double cashBalance) {
    try {
        work W(*conn);
        string sql = "UPDATE portfolio SET name = " + W.quote(name) +
                     ", cash_balance = " + W.quote(to_string(cashBalance)) +
                     " WHERE portfolio_id = " + W.quote(to_string(portfolioID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Portfolio updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void PortfolioManager::getPortfolio(int portfolioID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM portfolio WHERE portfolio_id = " + N.quote(to_string(portfolioID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No portfolio found with portfolio_id: " << portfolioID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "portfolio_id: " << c["portfolio_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
                cout << "cash_balance: " << c["cash_balance"].as<double>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving portfolio: " << e.what() << std::endl;
    }
}







void PortfolioManager::viewPortfolioByUserid(int userID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM portfolio WHERE user_id = " + N.quote(to_string(userID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No portfolios found for user_id: " << userID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "portfolio_id: " << c["portfolio_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
                cout << "cash_balance: " << c["cash_balance"].as<double>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving portfolios for user_id: " << e.what() << std::endl;
    }
}


// 计算每日回报率
vector<double> PortfolioManager::calculateReturns(const vector<double>& prices) {
    vector<double> returns;
    for (size_t i = 1; i < prices.size(); ++i) {
        double ret = (prices[i] - prices[i-1]) / prices[i-1];
        returns.push_back(ret);
    }
    return returns;
}

// 计算平均值
double PortfolioManager::mean(const vector<double>& data) {
    return accumulate(data.begin(), data.end(), 0.0) / data.size();
}

// 计算标准差
double PortfolioManager::standardDeviation(const vector<double>& data, double mean) {
    double sum = 0.0;
    for (double value : data) {
        sum += pow(value - mean, 2);
    }
    return sqrt(sum / data.size());
}

// 计算协方差
double PortfolioManager::covariance(const vector<double>& data1, const vector<double>& data2, double mean1, double mean2) {
    double sum = 0.0;
    for (size_t i = 0; i < data1.size(); ++i) {
        sum += (data1[i] - mean1) * (data2[i] - mean2);
    }
    return sum / data1.size();
}


void PortfolioManager::viewStocksInList(int portfolioId) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT s.stock_id, s.symbol, s.company_name, s.current_price, l.shares "
                     "FROM stock s JOIN stock_holding l ON s.stock_id = l.stock_id "
                     "WHERE l.portfolio_id = " + N.quote(to_string(portfolioId)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stocks found in Portfolio_Id: " << portfolioId << endl;
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


void PortfolioManager::calculateCovarianceMatrix(int portfolioId) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT s.stock_id, s.symbol, l.shares "
                     "FROM stock s JOIN stock_holding l ON s.stock_id = l.stock_id "
                     "WHERE l.portfolio_id = " + N.quote(to_string(portfolioId)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No stocks found in portfolioId: " << portfolioId << endl;
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
        cout << "Covariance Matrix for Stock PortfolioId: " << portfolioId << endl;
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

void PortfolioManager::printStocksInPortfolio(int portfolioID) {
    try {
        nontransaction N(*conn);

        // 获取 portfolio 中的所有股票
        string query = "SELECT s.stock_id, s.symbol, s.company_name, s.current_price, h.shares "
                       "FROM stock s JOIN stock_holding h ON s.stock_id = h.stock_id "
                       "WHERE h.portfolio_id = " + to_string(portfolioID) + ";";
        result R(N.exec(query));

        if (R.empty()) {
            cout << "No stocks found in portfolio " << portfolioID << endl;
        } else {
            cout << "Stocks in portfolio " << portfolioID << ":\n";
            for (const auto& row : R) {
                cout << "Stock ID: " << row["stock_id"].as<int>()
                     << ", Symbol: " << row["symbol"].as<string>()
                     << ", Company Name: " << row["company_name"].as<string>()
                     << ", Current Price: " << row["current_price"].as<double>()
                     << ", Shares: " << row["shares"].as<int>()
                     << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving stocks: " << e.what() << std::endl;
    }
}
