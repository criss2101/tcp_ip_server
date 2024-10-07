# Compiler and compilation flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./inc

LIB_FULL_NAME = lib/libserver.a
SRC_DIR = src
OBJ_DIR = obj
SRCS = $(SRC_DIR)/TcpIpServer.cpp
OBJS = $(OBJ_DIR)/TcpIpServer.o

# Build target for the static library
all: $(LIB_FULL_NAME)

# The static library creating
$(LIB_FULL_NAME): $(OBJS)
	@echo "Building static library $(LIB_FULL_NAME)..."
	mkdir -p lib
	ar rcs $(LIB_FULL_NAME) $(OBJS)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) lib

.PHONY: all clean
