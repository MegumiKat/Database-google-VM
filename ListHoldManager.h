//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_LISTHOLDMANAGER_H
#define DATABASE_GOOGLE_VM_LISTHOLDMANAGER_H


#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <unordered_map>

class ListHoldManager {
public:
    ListHoldManager();
    ~ListHoldManager();
    void createListHold(int listID, int stockID, int shares);
    void deleteListHold(int lHoldingID);
    void updateListHold(int lHoldingID, int listID, int stockID, int shares);
    void getListHold(int lHoldingID);
    void viewStocksInList(int listID);
    void calculateCovarianceMatrix(int listID);

private:
    pqxx::connection *conn;
    std::vector<double> calculateReturns(const std::vector<double>& prices);
    double mean(const std::vector<double>& data);
    double standardDeviation(const std::vector<double>& data, double mean);
    double covariance(const std::vector<double>& data1, const std::vector<double>& data2, double mean1, double mean2);
};

#endif //DATABASE_GOOGLE_VM_LISTHOLDMANAGER_H
