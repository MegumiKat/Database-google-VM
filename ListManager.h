//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_LISTMANAGER_H
#define DATABASE_GOOGLE_VM_LISTMANAGER_H


#include <iostream>
#include <pqxx/pqxx>
#include <string>

class ListManager {
public:
    ListManager();
    ~ListManager();
    void createList(int userID, const std::string& name, bool isPublic);
    void deleteList(int listID, int userID);
    void updateList(int listID, const std::string& name, bool isPublic);
    void getList(int listID);
    bool viewMyStockLists(int userID);
    bool viewAllPublicStockLists();
    void shareStockListWithFriend(int listID, int friendUserID);
    void getAllStocks(int limit);

private:
    pqxx::connection *conn;
};

#endif // DATABASE_GOOGLE_VM_LISTMANAGER_H
