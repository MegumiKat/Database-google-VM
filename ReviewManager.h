//
// Created by zhanglianhao2003 on 8/2/24.
//

#ifndef DATABASE_GOOGLE_VM_REVIEWMANAGER_H
#define DATABASE_GOOGLE_VM_REVIEWMANAGER_H



#include <iostream>
#include <pqxx/pqxx>
#include <string>

class ReviewManager {
public:
    ReviewManager();
    ~ReviewManager();
    void createReview(int listID, int userID, const std::string& reviewText);
    void deleteReview(int reviewID, int userID);
    void updateReview(int reviewID, const std::string& reviewText);
    void getReview(int reviewID);
    void viewAllStockLists();
    void viewReviewsByListId(int listID);

private:
    pqxx::connection *conn;
};

#endif // REVIEWMANAGER_H
