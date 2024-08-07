//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "ReviewManager.h"
#include <chrono>

using namespace std;
using namespace pqxx;

ReviewManager::ReviewManager() {
    try {
        conn = new connection("dbname=mydb user=postgres password=123 hostaddr=127.0.0.1 port=5432");
        if (conn->is_open()) {
            //cout << "Opened database successfully: " << conn->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

ReviewManager::~ReviewManager() {
    conn->disconnect();
    delete conn;
}

void ReviewManager::createReview(int listID, int userID, const string& reviewText) {
    try {
        nontransaction N(*conn);

        // 检查是否已经存在相同 listID 和 userID 的评论
        string checkQuery = "SELECT review_text FROM review WHERE list_id = " + N.quote(to_string(listID)) +
                            " AND user_id = " + N.quote(to_string(userID)) + ";";
        result checkResult(N.exec(checkQuery));

        if (!checkResult.empty()) {
            string existingReview = checkResult[0]["review_text"].as<string>();
            cout << "Existing review: " << existingReview << endl;
            cout << "Cannot add duplicate review for the same user and list." << endl;
            return;
        }

        // 结束非事务操作
        N.abort();

        // 如果没有重复评论，则插入新评论
        work W(*conn);
        string insertQuery = "INSERT INTO review (list_id, user_id, timestamp, review_text) VALUES (" +
                             W.quote(to_string(listID)) + ", " + W.quote(to_string(userID)) + ", NOW(), " + W.quote(reviewText) + ");";
        W.exec(insertQuery);
        W.commit();
        cout << "Review created successfully" << endl;

    } catch (const std::exception &e) {
        cerr << "Error: " << e.what() << std::endl;
    }
}



void ReviewManager::deleteReview(int reviewID, int userID) {
    try {
        nontransaction N(*conn);

        // 获取 review 的 user_id 和 list_id
        string reviewQuery = "SELECT user_id, list_id FROM review WHERE review_id = " + N.quote(to_string(reviewID)) + ";";
        result reviewResult(N.exec(reviewQuery));

        if (reviewResult.empty()) {
            cout << "Review not found" << endl;
            return;
        }

        int reviewUserID = reviewResult[0]["user_id"].as<int>();
        int listID = reviewResult[0]["list_id"].as<int>();

        // 检查 review 的 user_id 是否等于传入的 userID
        if (reviewUserID == userID) {
            N.abort();  // 结束非事务操作
            work W(*conn);
            string deleteQuery = "DELETE FROM review WHERE review_id = " + W.quote(to_string(reviewID)) + ";";
            W.exec(deleteQuery);
            W.commit();
            cout << "Review deleted successfully" << endl;
            return;
        }

        // 如果 review 的 user_id 不等于传入的 userID，检查 list 的 user_id
        string listQuery = "SELECT user_id FROM stock_list WHERE list_id = " + N.quote(to_string(listID)) + ";";
        result listResult(N.exec(listQuery));

        if (!listResult.empty() && listResult[0]["user_id"].as<int>() == userID) {
            N.abort();  // 结束非事务操作
            work W(*conn);
            string deleteQuery = "DELETE FROM review WHERE review_id = " + W.quote(to_string(reviewID)) + ";";
            W.exec(deleteQuery);
            W.commit();
            cout << "Review deleted successfully" << endl;
            return;
        }

        // 如果都不满足，打印错误信息
        cout << "You do not have permission to delete this review" << endl;

    } catch (const std::exception &e) {
        cerr << "Error: " << e.what() << std::endl;
    }
}



void ReviewManager::updateReview(int reviewID, const string& reviewText) {
    try {
        work W(*conn);
        string sql = "UPDATE review SET review_text = " + W.quote(reviewText) +
                     ", timestamp = NOW() WHERE review_id = " + W.quote(to_string(reviewID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Review updated successfully" << endl;
    } catch (const std::exception &e) {
        cerr << "Update failed: " << e.what() << std::endl;
    }
}

void ReviewManager::getReview(int reviewID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM review WHERE review_id = " + N.quote(to_string(reviewID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No review found with review_id: " << reviewID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "review_id: " << c["review_id"].as<int>() << endl;
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "timestamp: " << c["timestamp"].as<string>() << endl;
                cout << "review_text: " << c["review_text"].as<string>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving review: " << e.what() << std::endl;
    }
}
void ReviewManager::viewAllStockLists() {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM stock_list WHERE public = true;";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No public stock lists found." << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "name: " << c["name"].as<string>() << endl;
                cout << "public: " << c["public"].as<bool>() << endl;
                cout << "---------------------------------------" << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving stock lists: " << e.what() << std::endl;
    }
}
void ReviewManager::viewReviewsByListId(int listID) {
    try {
        nontransaction N(*conn);
        string sql = "SELECT * FROM review WHERE list_id = " + N.quote(to_string(listID)) + ";";
        result R(N.exec(sql));

        if (R.empty()) {
            cout << "No reviews found for list_id: " << listID << endl;
        } else {
            for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
                cout << "review_id: " << c["review_id"].as<int>() << endl;
                cout << "list_id: " << c["list_id"].as<int>() << endl;
                cout << "user_id: " << c["user_id"].as<int>() << endl;
                cout << "review_text: " << c["review_text"].as<string>() << endl;
            }
        }
    } catch (const std::exception &e) {
        cerr << "Error retrieving reviews for list_id: " << e.what() << std::endl;
    }
}