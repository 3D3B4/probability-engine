CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = test_probability_space.out
SOURCES = test_probability_space.cpp

all: $(TARGET)
	@./test_probability_space.out

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)



clean:
	rm -f $(TARGET)

