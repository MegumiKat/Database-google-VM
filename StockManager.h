//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_STOCKMANAGER_H
#define DATABASE_GOOGLE_VM_STOCKMANAGER_H

#include <iostream>
#include <pqxx/pqxx>
#include <string>

class StockManager {
public:
    StockManager();
    ~StockManager();
    void createStock(const std::string& symbol, const std::string& companyName);
    void deleteStock(int stockID);
    void updateStock(int stockID, const std::string& symbol, const std::string& companyName);
    void getStock(int stockID);

private:
    pqxx::connection *conn;
};

#endif //DATABASE_GOOGLE_VM_STOCKMANAGER_H
