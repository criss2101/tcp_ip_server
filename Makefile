# Compiler and compilation flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./Server

LIB_SERVER = Server/lib/libserver.a
LIB_SOCKETS = Sockets/lib/libsockets.a
TARGET = tcpip_server

# Build all targets
all: $(LIB_SOCKETS) $(LIB_SERVER) $(TARGET)

# Rule to build the main executable
$(TARGET): main.cpp
	@echo "Building executable $(TARGET)..."
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET) -L./Server/lib -lserver -L./Sockets/lib -lsockets -lpthread

# Rule to build libsockets
$(LIB_SOCKETS):
	@echo "Building library $(LIB_SOCKETS)..."
	$(MAKE) -C Sockets

# Rule to build libserver
$(LIB_SERVER):
	@echo "Building library $(LIB_SERVER)..."
	$(MAKE) -C Server

# Clean up all built files
clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
	$(MAKE) -C Server clean
	$(MAKE) -C Sockets clean

.PHONY: all clean
