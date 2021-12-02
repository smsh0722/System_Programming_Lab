# Makefile
TARGET=client server
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=12_client.o 12_server.o

all: $(TARGET)

client: 12_client.o
	$(CXX) $(CXXFLAGS) -o client 12_client.o
server: 12_server.o
	$(CXX) $(CXXFLAGS) -o server 12_server.o

12_client.o: 12_client.c
	$(CXX) $(CXXFLAGS) -c $^
12_server.o: 12_server.c
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm $(OBJECTS) $(TARGET)

