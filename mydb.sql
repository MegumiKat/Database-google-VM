\c mydb
-- User Table
CREATE TABLE users (
                      user_id SERIAL PRIMARY KEY,
                      user_name VARCHAR(50) NOT NULL,
                      password VARCHAR(50) NOT NULL,
                      email VARCHAR(100),
                      name VARCHAR(100)
);

-- Friendship Table
CREATE TABLE friend (
                        friend_id SERIAL PRIMARY KEY,
                        user_id1 INT,
                        user_id2 INT,
                        status VARCHAR(20),
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                        FOREIGN KEY (user_id1) REFERENCES users(user_id),
                        FOREIGN KEY (user_id2) REFERENCES users(user_id)
);

-- Portfolio Table
CREATE TABLE portfolio (
                           user_id INT,
                           portfolio_id SERIAL PRIMARY KEY,
                           name VARCHAR(100),
                           cash_balance DECIMAL(15, 2),
                           FOREIGN KEY (user_id) REFERENCES users(user_id)
);

-- Stock Table
CREATE TABLE stock (
                       stock_id SERIAL PRIMARY KEY,
                       symbol VARCHAR(10) UNIQUE,
                       company_name VARCHAR(100),
                       current_price DOUBLE PRECISION
);

-- Stock Holding Table
CREATE TABLE stock_holding (
                               holding_id SERIAL PRIMARY KEY,
                               portfolio_id INT,
                               stock_id INT,
                               shares INT,
                               FOREIGN KEY (portfolio_id) REFERENCES portfolio(portfolio_id),
                               FOREIGN KEY (stock_id) REFERENCES stock(stock_id)
);

-- Stock Daily Data Table
CREATE TABLE stock_daily_data (
                                  data_id SERIAL PRIMARY KEY,
                                  timestamp TIMESTAMP,
                                  open DECIMAL(15, 2),
                                  high DECIMAL(15, 2),
                                  low DECIMAL(15, 2),
                                  close DECIMAL(15, 2),
                                  volume INT,
                                  symbol VARCHAR(10)
);

-- Stock List Table
CREATE TABLE stock_list (
                            list_id SERIAL PRIMARY KEY,
                            user_id INT,
                            name VARCHAR(100),
                            public BOOLEAN DEFAULT FALSE,
                            FOREIGN KEY (user_id) REFERENCES users(user_id)
);

-- Stock List Holding Table
CREATE TABLE stock_list_holding (
                                    l_holding_id SERIAL PRIMARY KEY,
                                    list_id INT,
                                    stock_id INT,
                                    shares INT,
                                    FOREIGN KEY (list_id) REFERENCES stock_list(list_id),
                                    FOREIGN KEY (stock_id) REFERENCES stock(stock_id)
);

-- Review Table
CREATE TABLE review (
                        review_id SERIAL PRIMARY KEY,
                        list_id INT,
                        user_id INT,
                        timestamp TIMESTAMP,
                        review_text TEXT,
                        FOREIGN KEY (list_id) REFERENCES stock_list(list_id),
                        FOREIGN KEY (user_id) REFERENCES users(user_id)
);

CREATE TABLE statics (
                         Stat_id SERIAL PRIMARY KEY,
                         portfolio_id INTEGER,
                         COV JSONB,
                         Beta JSONB,
                         Matrix DOUBLE PRECISION,
                         FOREIGN KEY (portfolio_id) REFERENCES portfolio(portfolio_id)
);
