# Makefile
TARGET=ex11
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=ex11.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lpthread
ex11.o: ex11.c
	$(CXX) $(CXXFLAGS) -c $^ -lpthread
clean:
	rm $(OBJECTS) $(TARGET)