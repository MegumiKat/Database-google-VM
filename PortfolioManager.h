//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_PORTFOLIO_H
#define DATABASE_GOOGLE_VM_PORTFOLIO_H

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <unordered_map>

class PortfolioManager {
public:
    PortfolioManager();
    ~PortfolioManager();
    void createPortfolio(int userID, const std::string& name, double cashBalance);
    void deletePortfolio(int portfolioID);
    void updatePortfolio(int portfolioID, const std::string& name, double cashBalance);
    void getPortfolio(int portfolioID);
    void viewPortfolioByUserid(int userID);
    void calculateCovarianceMatrix(int portfolioId);
    void printStocksInPortfolio(int portfolioID);
    void viewStocksInList(int listID);

private:
    pqxx::connection *conn;
    std::vector<double> calculateReturns(const std::vector<double>& prices);
    double mean(const std::vector<double>& data);
    double standardDeviation(const std::vector<double>& data, double mean);
    double covariance(const std::vector<double>& data1, const std::vector<double>& data2, double mean1, double mean2);
};

#endif //DATABASE_GOOGLE_VM_PORTFOLIO_H
