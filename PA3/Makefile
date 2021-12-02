# Makefile
TARGET=server
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=server.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lpthread
server.o: server.c
	$(CXX) $(CXXFLAGS) -c $^ -lpthread
clean:
	rm $(OBJECTS) $(TARGET)