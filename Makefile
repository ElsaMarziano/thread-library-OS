# Variables
CXX = g++
AR = ar
CXXFLAGS = -Wall -g -std=c++11
ARFLAGS = rcs
TARGET = libuthreads.a
OBJECTS = uthreads.o thread.o

# Default target
all: $(TARGET)

# Rule to create the static library
$(TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJECTS)

# Rules to compile the object files
uthreads.o: uthreads.cpp uthreads.h
	$(CXX) $(CXXFLAGS) -c uthreads.cpp -o uthreads.o

thread.o: thread.cpp thread.h
	$(CXX) $(CXXFLAGS) -c thread.cpp -o thread.o

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean