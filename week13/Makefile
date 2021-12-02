# Makefile
TARGET=montecarlo
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=ex13.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lpthread
ex13.o: ex13.c
	$(CXX) $(CXXFLAGS) -c $^ -lpthread
clean:
	rm $(OBJECTS) $(TARGET)