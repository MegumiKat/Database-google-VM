//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_FRIENDMANAGER_H
#define DATABASE_GOOGLE_VM_FRIENDMANAGER_H


#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class FriendManager {
public:
    FriendManager();
    ~FriendManager();
    void sendFriendRequest(int userID1, int userID2);
    void acceptFriendRequest(int userID, int friendID); // 修改函数声明，增加 userID 参数
    void rejectFriendRequest(int userID, int friendID); // 新增函数声明
    void viewIncomingRequests(int userID);
    void viewOutgoingRequests(int userID);
    void cancelOutgoingRequest(int userID, int friendID); // 修改函数声明，增加 userID 参数
    void deleteFriend(int userID, int friendID); // 修改函数声明，增加 userID 参数
    void viewFriends(int userID);

private:
    pqxx::connection *conn;
};
#endif //DATABASE_GOOGLE_VM_FRIENDMANAGER_H
