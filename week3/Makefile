# Makefile
TARGET=w3
CXX=gcc
CXXFLAGS=-Wall
OBJECTS=main.o minus.o plus.o
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFALGS) -o $(TARGET) $(OBJECTS)
main.o: main.c
	$(CXX) $(CXXFLAGS) -c $^
plus.o: plus.c
	$(CXX) $(CXXFLAGS) -c $^
minus.o: minus.c
	$(CXX) $(CXXFLAGS) -c $^
clean:
	rm $(OBJECTS) $(TARGET)
