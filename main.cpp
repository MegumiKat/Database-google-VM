//
// Created by zhanglianhao2003 on 8/2/24.
//

#include "UserManager.h"
#include "PortfolioManager.h"
#include "FriendManager.h"
#include "ReviewManager.h"
#include "ListManager.h"
#include "StockManager.h"
#include "DailyManager.h"
#include "HoldManager.h"
#include "ListHoldManager.h"
#include "main.h"


using namespace std;
using namespace pqxx;



void registerUser(UserManager& userManager) {
    string username, password, email, name;
    cout << "Register a new user" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    cout << "Email: ";
    cin >> email;
    cout << "Name: ";
    cin >> name;
    userManager.createUser(username, password, email, name);
}

int loginUser(UserManager& userManager) {
    string username, password;
    cout << "Login" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    return userManager.login(username, password);
}



void PortfolioChosen(PortfolioManager& portfolioManager){
    int operation;
    string choice;
    bool login = true;
    while (login){
        cout << "\n--------------------------" << endl;
        cout << "Enter:\n Portfolio id to check Statics or 0 to Back" << endl;
        cin >> operation;
        if(operation != 0){
            portfolioManager.calculateTotalPortfolioValue(operation);
            portfolioManager.calculateCovarianceMatrix(operation);
            portfolioManager.viewStocksInList(operation);
        }else{
            cout << "\n--------------------------" << endl;
            login = false;
        }

    }
}

void StockReviewChosen(ReviewManager& reviewManager){
    int operation;
    bool login = true;
    while (login){
        cout << "\n--------------------------" << endl;
        cout << "Enter Stock id to check all Review List or 0 to Back\n"<< endl;
        cin >> operation;
        if(operation != 0){
            reviewManager.viewReviewsByListId(operation);
        }else{
            cout << "\n--------------------------" << endl;
            login= false;
        }

    }
}




void PortfolioOperation(PortfolioManager& portfolioManager, int userid, HoldManager& holdManager){
    int operation;
    string PortfolioName;
    double cashBalance;
    int portfolioId;
    int stockId;
    int share;
    int num;

    bool login = true;
    while (login){
        cout << "Enter:\n 1 for View Portfolio\n 2 for Create Portfolio\n 3 for Update Portfolio\n 4 for Delete Portfolio\n 5 for Add Stock_Holding\n 6 for sell Stock_Holding \n0 to Back" << endl;
        cin >> operation;
        switch (operation) {
                case 1:
                    cout << "\n--------------------------" << endl;
                    cout << "View Portfolio selected." << endl;
                    portfolioManager.viewPortfolioByUserid(userid);
                    PortfolioChosen(portfolioManager);
                    break;
                case 2:
                    cout << "\n--------------------------" << endl;
                    cout << "Create Portfolio selected." << endl;
                    cout << "Enter Portfolio Name";
                    cin >> PortfolioName;
                    cout << "Enter cash Balance: ";
                    cin >> cashBalance;
                    portfolioManager.createPortfolio(userid, PortfolioName, cashBalance);
                    break;
                case 3:
                    cout << "\n--------------------------" << endl;
                    cout << "Update Portfolio selected." << endl;
                    cout << "Enter Portfolio Id or 0 for back" << endl;
                    cin >> portfolioId;
                    if(portfolioId == 0){
                        break;
                    }
                    cout << "Enter Portfolio Name: ";
                    cin >> PortfolioName;
                    cout << "Enter cash Balance: ";
                    cin >> cashBalance;
                    portfolioManager.updatePortfolio(portfolioId,PortfolioName,cashBalance);
                    break;
                case 4:
                    cout << "\n--------------------------" << endl;
                    cout << "Delete Portfolio selected." << endl;
                    cout << "Enter Portfolio Id or 0 for back." << endl;
                    cin >> portfolioId;
                    if(portfolioId == 0){
                        break;
                    }
                    portfolioManager.deletePortfolio(portfolioId);
                    break;
                case 5:
                    cout << "\n--------------------------" << endl;
                    cout << "Add Stock_Holding" << endl;
                    cout << "Enter the number of stocks you want to see: " << endl;
                    cin >> num;
                    portfolioManager.getAllStocks(num);
                    cout << "Enter Portfolio Id or 0 for back" << endl;
                    cin >> portfolioId;
                    if(portfolioId == 0){
                        break;
                    }
                    cout << "Enter stock id: ";
                    cin >> stockId;
                    cout << "Number of Shares: ";
                    cin >> share;
                    holdManager.createHolding(portfolioId,stockId,share);
                    break;
                case 6:
                    cout << "\n--------------------------" << endl;
                    cout << "Sell Stock_Holding" << endl;
                    cout << "Enter Sell Portfolio Id or 0 for back" << endl;
                    cin >> portfolioId;
                    if(portfolioId == 0){
                        break;
                    }
                    cout << "Enter Sell stock id: ";
                    cin >> stockId;
                    cout << "Number of Sells: ";
                    cin >> share;
                    holdManager.sellHolding(portfolioId,stockId,share);
                    break;
                case 0:
                    cout << "\n--------------------------" << endl;
                    cout << "Come Back." << endl;
                    login = false;
                    break;
                default:
                    cout << "Invalid choice. Please enter a number between 0 and 5." << endl;
                    break;
            }

    }
}

void manageFriends(FriendManager& friendManager, int userID) {
    int operation;
    bool continueManaging = true;

    while (continueManaging) {
        cout << "Enter:\n 1 to Send Friend Request\n 2 to Accept Friend Request\n 3 to Reject Friend Request\n 4 to View Incoming Requests\n 5 to View Outgoing Requests\n 6 to Cancel Outgoing Request\n 7 to Delete Friend\n 8 to View Friends\n 0 to Back" << endl;
        cin >> operation;

        switch (operation) {
            case 1: {
                cout << "\n--------------------------" << endl;
                int friendUserID;
                cout << "Enter User ID to send friend request: ";
                cin >> friendUserID;
                friendManager.sendFriendRequest(userID, friendUserID);
                break;
            }
            case 2: {
                cout << "\n--------------------------" << endl;
                int friendID;
                cout << "Enter Friend ID to accept friend request: ";
                cin >> friendID;
                friendManager.acceptFriendRequest(userID, friendID);
                break;
            }
            case 3: {
                cout << "\n--------------------------" << endl;
                int friendID;
                cout << "Enter Friend ID to reject friend request: ";
                cin >> friendID;
                friendManager.rejectFriendRequest(userID, friendID);
                break;
            }
            case 4:
                cout << "\n--------------------------" << endl;
                friendManager.viewIncomingRequests(userID);
                break;
            case 5:
                cout << "\n--------------------------" << endl;
                friendManager.viewOutgoingRequests(userID);
                break;
            case 6: {
                cout << "\n--------------------------" << endl;
                int friendID;
                cout << "Enter Friend ID to cancel outgoing request: ";
                cin >> friendID;

                friendManager.cancelOutgoingRequest(userID, friendID);
                break;
            }
            case 7: {
                cout << "\n--------------------------" << endl;
                int friendID;
                cout << "Enter Friend ID to delete friend: ";
                cin >> friendID;
                friendManager.deleteFriend(userID, friendID);
                break;
            }
            case 8:
                cout << "\n--------------------------" << endl;
                friendManager.viewFriends(userID);
                break;
            case 0:
                cout << "\n--------------------------" << endl;
                continueManaging = false;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 0 and 8." << endl;
                break;
        }

    }
}


void Reviewing(ReviewManager& reviewManager, int userID){
    int operation;
    int reviewID;
    int listID;
    string reviewText;

    bool continueReview = true;
    while (continueReview){
        cout << "Enter:\n 1 for View Stock List\n 2 for Create Review\n 3 for Update Review\n 4 for Delete Review\n 0 to Back" << endl;
        cin >> operation;
        switch (operation) {
            case 1:
                cout << "\n--------------------------" << endl;
                cout << "View All Stock List." << endl;
                reviewManager.viewAllStockLists();
                StockReviewChosen(reviewManager);
                break;
            case 2:
                cout << "\n--------------------------" << endl;
                cout << "Create Review selected." << endl;
                cout << "Enter Stock List ID: ";
                cin >> listID;
                cout << "Enter Review Text: ";
                cin.ignore(); // 忽略前一个输入后的换行符
                getline(cin, reviewText);
                reviewManager.createReview(listID, userID, reviewText);
                break;
            case 3:
                cout << "\n--------------------------" << endl;
                cout << "Update Review selected." << endl;
                cout << "Enter Review ID or 0 to back: ";
                cin >> reviewID;
                if (reviewID == 0) {
                    break;
                }
                cout << "Enter Updated Review Text: ";
                cin.ignore(); // 忽略前一个输入后的换行符
                getline(cin, reviewText);
                reviewManager.updateReview(reviewID, reviewText);
                break;
            case 4:
                cout << "\n--------------------------" << endl;
                cout << "Delete Review selected." << endl;
                cout << "Enter Review ID or 0 to back: ";
                cin >> reviewID;
                if (reviewID == 0) {
                    break;
                }
                reviewManager.deleteReview(reviewID, userID);
                break;
            case 0:
                cout << "\n--------------------------" << endl;
                cout << "Going Back." << endl;
                continueReview = false;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 0 and 4." << endl;
                break;
        }

    }
}

void stockListChosen(ListHoldManager& listHoldManager){
    int operation;
    bool login = true;
    while (login){
        cout << "\n--------------------------" << endl;
        cout << "Enter List id to check all Stock inside or 0 to Back\n"<< endl;
        cin >> operation;
        if(operation != 0){
            listHoldManager.calculateCovarianceMatrix(operation);
            listHoldManager.viewStocksInList(operation);
        }else{
            cout << "\n--------------------------" << endl;
            login= false;
        }

    }
}


void stockListOperation(DailyManager& dailyManager, StockManager& stockManager,ListManager& ListManager, ListHoldManager& listHoldManager, int userID){
    int operation;
    int num;
    bool login = true;
    while (login) {
        cout << "Enter:\n"
             << "1 for View My Stock Lists\n"
             << "2 for View All Public Stock Lists\n"
             << "3 for Create Stock List\n"
             << "4 for Update Stock List\n"
             << "5 for Share Stock List to Friend\n"
             << "6 for Add Stock to List\n"
             << "7 for integrate stock info\n"
             << "8 for delete stock list\n"
             << "0 for Back\n";
        cin >> operation;

        switch (operation) {
            case 1:
                cout << "\n--------------------------" << endl;
                if(ListManager.viewMyStockLists(userID)){
                    stockListChosen(listHoldManager);
                }
                break;
            case 2:
                cout << "\n--------------------------" << endl;
                if(ListManager.viewAllPublicStockLists()){
                    stockListChosen(listHoldManager);
                }
                break;
            case 3: {
                cout << "\n--------------------------" << endl;
                string listName;
                bool isPublic;
                cout << "Enter Stock List Name: ";
                cin.ignore();
                getline(cin, listName);
                cout << "Is Public (1 for Yes, 0 for No): ";
                cin >> isPublic;
                ListManager.createList(userID, listName, isPublic);
                break;
            }
            case 4: {
                cout << "\n--------------------------" << endl;
                int listID;
                string listName;
                bool isPublic;
                cout << "Enter Stock List ID to Update: ";
                cin >> listID;
                cout << "Enter New Stock List Name: ";
                cin.ignore();
                getline(cin, listName);
                cout << "Is Public (1 for Yes, 0 for No): ";
                cin >> isPublic;
                ListManager.updateList(listID, listName, isPublic);
                break;
            }
            case 5: {
                cout << "\n--------------------------" << endl;
                int listID, friendUserID;
                cout << "Enter Stock List ID to Share: ";
                cin >> listID;
                cout << "Enter Friend User ID: ";
                cin >> friendUserID;
                ListManager.shareStockListWithFriend(listID, friendUserID);
                break;
            }
            case 6: {
                cout << "\n--------------------------" << endl;
                int listID, stockID, shares;
                cout << "Enter number of stocks you want to see: ";
                cin >> num;
                ListManager.getAllStocks(num);
                cout << "Enter Stock List ID to Add Stock: ";
                cin >> listID;
                cout << "Enter Stock ID: ";
                cin >> stockID;
                cout << "Enter Number of Shares: ";
                cin >> shares;
                listHoldManager.createListHold(listID, stockID, shares);
                break;
            }
            case 7: {
                cout << "\n--------------------------" << endl;
                double open, high, low, close, volume;
                string symbol;
                cout << "Enter number of open price: ";
                cin >> open;
                cout << "Enter number of high price: ";
                cin >> high;
                cout << "Enter number of low price: ";
                cin >> low;
                cout << "Enter number of close price: ";
                cin >> close;
                cout << "Enter number of volume: ";
                cin >> volume;
                cout << "Enter the symbol: ";
                cin >> symbol;
                dailyManager.createDailyData(open, high,low, close, volume, symbol);
                break;
            }
            case 8: {
                int id;
                cout << "Enter the list ID you want to delete: \n";
                cin >> id;
                ListManager.deleteList(id, userID);
                break;
            }
            case 0:
                cout << "\n--------------------------" << endl;
                login = false;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 0 and 6." << endl;
                break;
        }

    }
}


void showOperations(PortfolioManager& portfolioManager, StockManager& stockManager, FriendManager& friendManager, ReviewManager& reviewManager, DailyManager& dailyManager, int userid, ListManager& listManager, ListHoldManager& listHoldManager, HoldManager holdManager) {
    int operation;
    int id;
    bool login = true;
    int num;
    cout << "Welcome to use C43 pro!\n";
    while (login){
        cout << "Enter:\n 1 for Portfolio Management\n 2 for StockList Management\n 3 for Friendship List\n 4 for Reviewing\n 5 for Prediction\n 6 for Stock history\n 0 to Logout" << endl;
        cin >> operation;
        switch (operation) {
                case 1:
                    cout << "\n--------------------------" << endl;
                    // Portfolio Management
                    cout << "Portfolio Management selected." << endl;
                    PortfolioOperation(portfolioManager,userid,holdManager );
                    // 在这里调用portfolioManager的相关功能
                    break;
                case 2:
                    cout << "\n--------------------------" << endl;
                    // StockList Management
                    cout << "StockList Management selected." << endl;
                    stockListOperation(dailyManager, stockManager,listManager,listHoldManager,userid);
                    // 在这里调用stockManager的相关功能
                    break;
                case 3:
                    cout << "\n--------------------------" << endl;
                    // Friendship List
                    cout << "Friendship List selected." << endl;
                    manageFriends(friendManager, userid);
                    // 在这里调用friendManager的相关功能
                    break;
                case 4:
                    cout << "\n--------------------------" << endl;
                    // Reviewing
                    cout << "Reviewing selected." << endl;
                    // 在这里调用reviewManager的相关功能
                     Reviewing(reviewManager,userid);
                    break;
                case 5:
                    cout << "\n--------------------------" << endl;
                    cout << "Prediction selected." << endl;
                    cout << "Enter stock ID: ";
                    cin >> id;
                    holdManager.checkStockPerformance(id);
                    break;
                case 6:
                    int option;
                    cout << "\n--------------------------" << endl;
                    cout << "Check history of specific stock" << endl;
                    cout << "Enter: \n1 for check history of stock without graph: \n2 for check history of stock with graph\n";
                    cin >> option;
                    if (option == 1){
                        cout << "Enter stock ID: ";
                        cin >> id;
                        cout << "Enter num of history data: ";
                        cin >> num;
                        cout << "\n--------------------------" << endl;
                        dailyManager.getDailyData(id, num);
                    }else if (option == 2) {
                        std::string startDate;
                        std::string endDate;

                        std::cout << "Enter stock ID to generate graph: ";
                        std::cin >> id;

                        std::cout << "Enter start date (YYYY-MM-DD): ";
                        std::cin >> startDate;

                        std::cout << "Enter end date (YYYY-MM-DD): ";
                        std::cin >> endDate;
                        holdManager.generateStockGraph(id, startDate, endDate);
                    }
                    break;
                case 0:
                    cout << "\n--------------------------" << endl;
                    cout << "Back." << endl;
                    login = false;
                    break;
                default:
                    cout << "Invalid choice. Please enter a number between 0 and 5." << endl;
                    break;
            }

    }

}



int main() {
    FriendManager friendManager;
    UserManager userManager;
    ListManager listManager;
    ReviewManager reviewManager;
    StockManager stockManager;
    DailyManager dailyManager;
    HoldManager holdManager;
    ListHoldManager listHoldManager;
    PortfolioManager PortfolioManager;

    try{
        int choice;
        int running = 1;
        int quit = -1;
        int userId;
        while (running) {
            cout << "Enter 1 to login, 2 to register, 0 to quit: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    userId = loginUser(userManager);
                    cout << userId << endl;
                    if (userId != -1) {
                        cout << "Login successful. User ID: " << userId << "\n" << endl;
                        // 在这里执行登录后的操作
                            cout << "\n--------------------------" << endl;
                            showOperations(PortfolioManager, stockManager, friendManager, reviewManager, dailyManager,userId, listManager,listHoldManager, holdManager);

                    } else {
                        cout << "Invalid username or password." << endl;
                    }
                    break;
                case 2:
                    registerUser(userManager);
                    break;
                case 0:
                    running = false;
                    break;
                default:
                    cout << "Invalid choice. Please enter 1, 2, or 0." << endl;
                    break;
            }
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}