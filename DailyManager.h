//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_DAILYMANAGER_H
#define DATABASE_GOOGLE_VM_DAILYMANAGER_H


#include <iostream>
#include <pqxx/pqxx>
#include <string>

class DailyManager {
public:
    DailyManager();
    ~DailyManager();
    void createDailyData(double open, double high, double low, double close, double volume, const std::string& symbol);
    void deleteDailyData(int dataID);
    void updateDailyData(int dataID, int stockID, const std::string& timestamp, double open, double high, double low, double close, int volume);
    void getDailyData(int dataID);
    void getDailyData(int stockID, int num);

private:
    pqxx::connection *conn;
};

#endif //DATABASE_GOOGLE_VM_DAILYMANAGER_H
