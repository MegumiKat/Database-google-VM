//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "HoldManager.h"


using namespace std;
using namespace pqxx;

HoldManager::HoldManager() {
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

HoldManager::~HoldManager() {
    conn->disconnect();
    delete conn;
}

void HoldManager::createHolding(int portfolioID, int stockID, int shares) {
    try {
        // 开始事务
        work W(*conn);

        // 查询股票的当前价格
        string priceQuery = "SELECT current_price FROM stock WHERE stock_id = " + W.quote(to_string(stockID)) + ";";
        result priceR(W.exec(priceQuery));

        if (priceR.empty()) {
            cerr << "Error: Stock with ID " << stockID << " not found" << endl;
            return;
        }

        double currentPrice = priceR[0]["current_price"].as<double>();

        // 计算总成本
        double totalCost = currentPrice * shares;

        // 获取当前现金余额
        string balanceQuery = "SELECT cash_balance FROM portfolio WHERE portfolio_id = " + W.quote(to_string(portfolioID)) + ";";
        result balanceR(W.exec(balanceQuery));

        if (balanceR.empty()) {
            cerr << "Error: Portfolio with ID " << portfolioID << " not found" << endl;
            return;
        }

        double currentBalance = balanceR[0]["cash_balance"].as<double>();

        if (currentBalance < totalCost) {
            cerr << "Error: Insufficient funds in portfolio" << endl;
            return;
        }

        // 更新现金余额
        double newBalance = currentBalance - totalCost;
        string updateBalanceQuery = "UPDATE portfolio SET cash_balance = " + W.quote(to_string(newBalance)) +
                                    " WHERE portfolio_id = " + W.quote(to_string(portfolioID)) + ";";
        W.exec(updateBalanceQuery);

        // 插入新的持仓记录
        string insertQuery = "INSERT INTO stock_holding (portfolio_id, stock_id, shares) VALUES (" +
                             W.quote(to_string(portfolioID)) + ", " + W.quote(to_string(stockID)) + ", " + W.quote(to_string(shares)) + ");";
        W.exec(insertQuery);

        // 提交事务
        W.commit();
        cout << "Holding created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}



void HoldManager::deleteHolding(int holdingID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM stock_holding WHERE holding_id = " + W.quote(to_string(holdingID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Holding deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void HoldManager::updateHolding(int holdingID, int shares) {
    try {
        work W(*conn);
        string sql = "UPDATE stock_holding SET shares = " + W.quote(to_string(shares)) +
                     " WHERE holding_id = " + W.quote(to_string(holdingID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Holding updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void HoldManager::getHolding(int holdingID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_holding WHERE holding_id = " + N.quote(to_string(holdingID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No holding found with holding_id: " << holdingID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "holding_id: " << c["holding_id"].as<int>() << endl;
                cout << "portfolio_id: " << c["portfolio_id"].as<int>() << endl;
                cout << "stock_id: " << c["stock_id"].as<int>() << endl;
                cout << "shares: " << c["shares"].as<int>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving holding: " << e.what() << std::endl;
    }
}
void HoldManager::sellHolding(int portfolioID, int stockID, int shares) {
    try {
        // 开始事务
        work W(*conn);

        // 查询持仓记录
        string holdingQuery = "SELECT holding_id, shares FROM stock_holding WHERE portfolio_id = " + W.quote(to_string(portfolioID)) +
                              " AND stock_id = " + W.quote(to_string(stockID)) + ";";
        result holdingR(W.exec(holdingQuery));

        if (holdingR.empty()) {
            cerr << "Error: No holding found for portfolio ID " << portfolioID << " and stock ID " << stockID << endl;
            return;
        }

        int holdingID = holdingR[0]["holding_id"].as<int>();
        int currentShares = holdingR[0]["shares"].as<int>();

        if (shares > currentShares) {
            cerr << "Error: Not enough shares to sell" << endl;
            return;
        }

        // 查询股票的当前价格
        string priceQuery = "SELECT current_price FROM stock WHERE stock_id = " + W.quote(to_string(stockID)) + ";";
        result priceR(W.exec(priceQuery));

        if (priceR.empty()) {
            cerr << "Error: Stock with ID " << stockID << " not found" << endl;
            return;
        }

        double currentPrice = priceR[0]["current_price"].as<double>();

        // 计算总收入
        double totalRevenue = currentPrice * shares;

        // 获取当前现金余额
        string balanceQuery = "SELECT cash_balance FROM portfolio WHERE portfolio_id = " + W.quote(to_string(portfolioID)) + ";";
        result balanceR(W.exec(balanceQuery));

        if (balanceR.empty()) {
            cerr << "Error: Portfolio with ID " << portfolioID << " not found" << endl;
            return;
        }

        double currentBalance = balanceR[0]["cash_balance"].as<double>();

        // 更新现金余额
        double newBalance = currentBalance + totalRevenue;
        string updateBalanceQuery = "UPDATE portfolio SET cash_balance = " + W.quote(to_string(newBalance)) +
                                    " WHERE portfolio_id = " + W.quote(to_string(portfolioID)) + ";";
        W.exec(updateBalanceQuery);

        if (shares == currentShares) {
            // 删除持仓记录
            string deleteQuery = "DELETE FROM stock_holding WHERE holding_id = " + W.quote(to_string(holdingID)) + ";";
            W.exec(deleteQuery);
        } else {
            // 更新持仓记录
            int remainingShares = currentShares - shares;
            string updateQuery = "UPDATE stock_holding SET shares = " + W.quote(to_string(remainingShares)) +
                                 " WHERE holding_id = " + W.quote(to_string(holdingID)) + ";";
            W.exec(updateQuery);
        }

        // 提交事务
        W.commit();
        cout << "Holding sold successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void HoldManager::checkStockPerformance(int stockID) {
    try {
        nontransaction N(*conn);

        // 查询 symbol
        string symbolQuery = "SELECT symbol FROM stock WHERE stock_id = " + to_string(stockID) + ";";
        result symbolR(N.exec(symbolQuery));

        if (symbolR.empty()) {
            cerr << "Error: Stock with ID " << stockID << " not found" << endl;
            return;
        }

        string symbol = symbolR[0]["symbol"].as<string>();

        // 查询最小和最大 timestamp 对应的 close 值
        string minTimestampQuery = "SELECT close FROM stock_daily_data WHERE symbol = " + N.quote(symbol) + " ORDER BY timestamp ASC LIMIT 1;";
        string maxTimestampQuery = "SELECT close FROM stock_daily_data WHERE symbol = " + N.quote(symbol) + " ORDER BY timestamp DESC LIMIT 1;";

        result minTimestampR(N.exec(minTimestampQuery));
        result maxTimestampR(N.exec(maxTimestampQuery));

        if (minTimestampR.empty() || maxTimestampR.empty()) {
            cerr << "Error: No data found for symbol " << symbol << endl;
            return;
        }

        double minClose = minTimestampR[0]["close"].as<double>();
        double maxClose = maxTimestampR[0]["close"].as<double>();

        if (maxClose - minClose < 0) {
            cout << "It is a bad stock" << endl;
        } else {
            cout << "It is a good stock" << endl;
        }

    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}