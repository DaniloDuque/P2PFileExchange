# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -I../googletest/googletest/include -I../googletest/googlemock/include -pthread

# Source files
SOURCES = unitTests.cpp  # Add your source files here
GTEST_SOURCES = ../googletest/googletest/src/gtest-all.cc

# Object files
OBJECTS = $(SOURCES:.cpp=.o) gtest-all.o

# Executable name
TARGET = test_executable

# Rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

gtest-all.o: $(GTEST_SOURCES)
	$(CXX) $(CXXFLAGS) -c $(GTEST_SOURCES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET) gtest-all.o

.PHONY: all clean

