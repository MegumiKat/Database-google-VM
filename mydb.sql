\c mydb
-- User Table
CREATE TABLE "User" (
                        "UserID" SERIAL PRIMARY KEY,
                        "UserName" VARCHAR(50) NOT NULL,
                        "Password" VARCHAR(50) NOT NULL,
                        "Email" VARCHAR(100),
                        "Name" VARCHAR(100)
);

-- Friendship Table
CREATE TABLE "Friend" (
                              "FriendID" SERIAL PRIMARY KEY,
                              "UserID1" INT,
                              "UserID2" INT,
                              "Status" VARCHAR(20),
                              FOREIGN KEY ("UserID1") REFERENCES "User"("UserID"),
                              FOREIGN KEY ("UserID2") REFERENCES "User"("UserID")
);

-- Portfolio Table
CREATE TABLE "Portfolio" (
                             "UserID" INT,
                             "ProfolioID" SERIAL PRIMARY KEY,
                             "Name" VARCHAR(100),
                             "CashBalance" DECIMAL(15, 2),
                             FOREIGN KEY ("UserID") REFERENCES "User"("UserID")
);

-- Stock Table
CREATE TABLE "Stock" (
                         "StockID" SERIAL PRIMARY KEY,
                         "Symbol" VARCHAR(10),
                         "CompanyName" VARCHAR(100)
);

-- StockHolding Table
CREATE TABLE "StockHolding" (
                                "HoldingID" SERIAL PRIMARY KEY,
                                "ProfolioID" INT,
                                "StockID" INT,
                                "Shares" INT,
                                FOREIGN KEY ("ProfolioID") REFERENCES "Portfolio"("ProfolioID"),
                                FOREIGN KEY ("StockID") REFERENCES "Stock"("StockID")
);

-- StockDailyData Table
CREATE TABLE "StockDailyData" (
                                  "DataID" SERIAL PRIMARY KEY,
                                  "StockID" INT,
                                  "Timestamp" TIMESTAMP,
                                  "Open" DECIMAL(15, 2),
                                  "High" DECIMAL(15, 2),
                                  "Low" DECIMAL(15, 2),
                                  "Close" DECIMAL(15, 2),
                                  "Volume" INT,
                                  FOREIGN KEY ("StockID") REFERENCES "Stock"("StockID")
);

-- StockList Table
CREATE TABLE "StockList" (
                             "ListID" SERIAL PRIMARY KEY,
                             "UserID" INT,
                             "Name" VARCHAR(100),
                             "Public" BOOLEAN DEFAULT FALSE,
                             FOREIGN KEY ("UserID") REFERENCES "User"("UserID")
);

-- StockListHolding Table
CREATE TABLE "StockListHolding" (
                                    "LHoldingID" SERIAL PRIMARY KEY,
                                    "ListID" INT,
                                    "StockID" INT,
                                    "Shares" INT,
                                    FOREIGN KEY ("ListID") REFERENCES "StockList"("ListID"),
                                    FOREIGN KEY ("StockID") REFERENCES "Stock"("StockID")
);

-- Review Table
CREATE TABLE "Review" (
                          "ReviewID" SERIAL PRIMARY KEY,
                          "ListID" INT,
                          "UserID" INT,
                          "Timestamp" TIMESTAMP,
                          "ReviewText" TEXT,
                          FOREIGN KEY ("ListID") REFERENCES "StockList"("ListID"),
                          FOREIGN KEY ("UserID") REFERENCES "User"("UserID")
);
