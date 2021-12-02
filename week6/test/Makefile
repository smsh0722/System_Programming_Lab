# Makefile
TARGET=crond
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=main.o

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
main.o: main.c
	$(CXX) $(CXXFALGS) -c $^
clean:
	rm $(OBJECTS) $(TARGET)
