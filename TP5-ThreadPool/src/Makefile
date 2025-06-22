# Compiler settings - Can change to clang++ if preferred
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -g


# Target
TARGET = threadpool
SRC = thread-pool.cc Semaphore.cc tpcustomtest.cc

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Clean up build artifacts
custom:
	$(CXX) $(CXXFLAGS) -o $(TARGET) thread-pool.cc Semaphore.cc test_custom.cc

clean:
	rm -f $(TARGET)

.PHONY: all clean run