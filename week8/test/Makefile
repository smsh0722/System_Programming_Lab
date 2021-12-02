# Makefile
TARGET=ex8
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=main.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
main.o: main.c
	$(CXX) $(CXXFLAGS) -c $^
clean:
	rm $(OBJECTS) $(TARGET)

