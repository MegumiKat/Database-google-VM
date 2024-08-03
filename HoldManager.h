//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_HOLDMANAGER_H
#define DATABASE_GOOGLE_VM_HOLDMANAGER_H

#include <iostream>
#include <pqxx/pqxx>
#include <string>

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

private:
    pqxx::connection *conn;
};


#endif //DATABASE_GOOGLE_VM_HOLDMANAGER_H
