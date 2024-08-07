//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_HOLDMANAGER_H
#define DATABASE_GOOGLE_VM_HOLDMANAGER_H

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>


class HoldManager {
public:
    HoldManager();
    ~HoldManager();
    void createHolding(int portfolioID, int stockID, int shares);
    void deleteHolding(int holdingID);
    void updateHolding(int holdingID, int shares);
    void getHolding(int holdingID);
    void sellHolding(int portfolioID, int stockID, int shares);
    void checkStockPerformance(int stockID);
    void generateStockGraph(int stockID, const std::string& startDate, const std::string& endDate);

private:
    pqxx::connection *conn;
    std::string getIntervalCondition(const std::string& startDate, const std::string& endDate);
};


#endif //DATABASE_GOOGLE_VM_HOLDMANAGER_H
