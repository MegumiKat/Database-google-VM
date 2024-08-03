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
        work W(*conn);
        string sql = "INSERT INTO review (list_id, user_id, timestamp, review_text) VALUES (" +
                     W.quote(to_string(listID)) + ", " + W.quote(to_string(userID)) + ", NOW(), " + W.quote(reviewText) + ");";
        W.exec(sql);
        W.commit();
        cout << "Review created successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void ReviewManager::deleteReview(int reviewID) {
    try {
        work W(*conn);
        string sql = "DELETE FROM review WHERE review_id = " + W.quote(to_string(reviewID)) + ";";
        W.exec(sql);
        W.commit();
        cout << "Review deleted successfully" << endl;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
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