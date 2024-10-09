TARGET = simple_client

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

SRCS = main.cpp
OBJS = main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) main.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

clean:
	rm -f main.o $(TARGET)

.PHONY: all clean
