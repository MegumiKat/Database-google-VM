#include "FriendManager.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace pqxx;

FriendManager::FriendManager() {
    try {
        conn = new connection("dbname=mydb user=postgres password=123 hostaddr=127.0.0.1 port=5432");
        if (conn->is_open()) {
            // cout << "Opened database successfully: " << conn->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

FriendManager::~FriendManager() {
    conn->disconnect();
    delete conn;
}

void FriendManager::sendFriendRequest(int userID1, int userID2) {
    try {
        // 使用事务对象来处理所有数据库操作
        work W(*conn);

        // 检查是否存在完全相同的 userID1 和 userID2 且状态不是 'rejected' 的记录
        string existingCheckQuery = "SELECT * FROM friend WHERE user_id1 = " + W.quote(to_string(userID1)) +
                                    " AND user_id2 = " + W.quote(to_string(userID2)) + " AND status != 'rejected';";
        result existingCheckR(W.exec(existingCheckQuery));

        if (!existingCheckR.empty()) {
            cout << "Can't send friend request as there is already an existing request or friendship." << endl;
            return;
        }

        // 检查是否在5分钟内被拒绝
        string checkQuery = "SELECT created_at FROM friend WHERE user_id1 = " + W.quote(to_string(userID1)) +
                            " AND user_id2 = " + W.quote(to_string(userID2)) + " AND status = 'rejected' " +
                            "AND created_at > NOW() - INTERVAL '5 minutes';";
        result checkR(W.exec(checkQuery));

        if (!checkR.empty()) {
            cout << "You cannot send a friend request to the same user within 5 minutes after being rejected." << endl;
            return;
        }

        // 插入新的好友请求记录
        string sql = "INSERT INTO friend (user_id1, user_id2, status, created_at) VALUES (" +
                     W.quote(to_string(userID1)) + ", " + W.quote(to_string(userID2)) + ", 'pending', NOW());";
        W.exec(sql);
        W.commit();
        cout << "Friend request sent successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}


void FriendManager::acceptFriendRequest(int userID, int friendID) {
    try {
        {
            // 确认 userID 是 user_id2，只有接收者可以同意
            nontransaction N(*conn);
            string checkQuery = "SELECT * FROM friend WHERE friend_id = " + N.quote(to_string(friendID)) +
                                " AND user_id2 = " + N.quote(to_string(userID)) + " AND status = 'pending';";
            result checkR(N.exec(checkQuery));

            if (checkR.empty()) {
                cerr << "You can only accept friend requests sent to you." << endl;
                return;
            }
        } // 确保 nontransaction 对象在这里被销毁

        work W(*conn);
        string sql = "UPDATE friend SET status = 'accepted', created_at = NOW() WHERE friend_id = " + W.quote(to_string(friendID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Friend request accepted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void FriendManager::rejectFriendRequest(int userID, int friendID) {
    try {
        {
            // 确认 userID 是 user_id2，只有接收者可以拒绝
            nontransaction N(*conn);
            string checkQuery = "SELECT * FROM friend WHERE friend_id = " + N.quote(to_string(friendID)) +
                                " AND user_id2 = " + N.quote(to_string(userID)) + " AND status = 'pending';";
            result checkR(N.exec(checkQuery));

            if (checkR.empty()) {
                cerr << "You can only reject friend requests sent to you." << endl;
                return;
            }
        } // 确保 nontransaction 对象在这里被销毁

        work W(*conn);
        string sql = "UPDATE friend SET status = 'rejected', created_at = NOW() WHERE friend_id = " + W.quote(to_string(friendID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Friend request rejected successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}


void FriendManager::viewIncomingRequests(int userID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM friend WHERE user_id2 = " + N.quote(to_string(userID)) + " AND status = 'pending';";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No incoming friend requests found." << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "friendship_id: " << c["friend_id"].as<int>() << endl;
                cout << "user_id1: " << c["user_id1"].as<int>() << endl;
                cout << "user_id2: " << c["user_id2"].as<int>() << endl;
                cout << "status: " << c["status"].as<string>() << endl;
                cout << "\n--------------------------" << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving incoming friend requests: " << e.what() << std::endl;
    }
}

void FriendManager::viewOutgoingRequests(int userID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM friend WHERE user_id1 = " + N.quote(to_string(userID)) + " AND status = 'pending';";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No outgoing friend requests found." << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "friendship_id: " << c["friend_id"].as<int>() << endl;
                cout << "user_id1: " << c["user_id1"].as<int>() << endl;
                cout << "user_id2: " << c["user_id2"].as<int>() << endl;
                cout << "status: " << c["status"].as<string>() << endl;
                cout << "\n--------------------------" << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving outgoing friend requests: " << e.what() << std::endl;
    }
}

void FriendManager::cancelOutgoingRequest(int userID, int friendID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM friend WHERE friend_id = " + W.quote(to_string(friendID)) +
                     " AND user_id1 = " + W.quote(to_string(userID)) + " AND status = 'pending';";
        result R(W.exec(sql));
        W.commit();

        if (R.affected_rows() > 0) {
            cout << "Outgoing friend request cancelled successfully" << endl;
        } else {
            cout << "No matching outgoing friend request found to cancel." << endl;
        }
    } catch (const std::exception &e) {
        cerr << "Cancel request failed: " << e.what() << std::endl;
    }
}


void FriendManager::deleteFriend(int userID, int friendID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM friend WHERE friend_id = " + W.quote(to_string(friendID)) +
                     " AND (user_id1 = " + W.quote(to_string(userID)) + " OR user_id2 = " + W.quote(to_string(userID)) + ") AND status = 'accepted';";
        result R(W.exec(sql));
        W.commit();

        if (R.affected_rows() > 0) {
            cout << "Friend deleted successfully" << endl;
        } else {
            cout << "No matching friend record found to delete." << endl;
        }
    } catch (const std::exception &e) {
        cerr << "Delete failed: " << e.what() << std::endl;
    }
}


void FriendManager::viewFriends(int userID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM friend WHERE (user_id1 = " + N.quote(to_string(userID)) + " OR user_id2 = " + N.quote(to_string(userID)) + ") AND status = 'accepted';";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No friends found." << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                int friendID = c["friend_id"].as<int>();
                int friendUserID = (c["user_id1"].as<int>() == userID) ? c["user_id2"].as<int>() : c["user_id1"].as<int>();
                cout << "friend_id: " << friendID << ", friend_user_id: " << friendUserID << endl;

            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving friends: " << e.what() << std::endl;
    }
}
