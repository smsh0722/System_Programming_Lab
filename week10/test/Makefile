# Makefile
TARGET=client server
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=client.o server.o

all: $(TARGET)

client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o
server: server.o
	$(CXX) $(CXXFLAGS) -o server server.o

client.o: client.c
	$(CXX) $(CXXFLAGS) -c $^
server.o: server.c
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm $(OBJECTS) $(TARGET)

