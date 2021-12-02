# Makefile
TARGET=ex14
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=ex14.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lpthread
ex14.o: ex14.c
	$(CXX) $(CXXFLAGS) -c $^ -lpthread
clean:
	rm $(OBJECTS) $(TARGET)