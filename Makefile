# 指定编译器
CXX = g++

# 指定编译选项
CXXFLAGS = -I/usr/include/pqxx -I/usr/include/eigen3

# 指定链接选项
LDFLAGS = -lpqxx -lpq

# 指定目标文件
TARGET = UserManager

# 指定源文件
SRCS = main.cpp UserManager.cpp PortfolioManager.cpp FriendManager.cpp ListManager.cpp ReviewManager.cpp StockManager.cpp DailyManager.cpp HoldManager.cpp ListHoldManager.cpp

# 生成的目标文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

# 编译源文件生成目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

