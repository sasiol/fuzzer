# Compiler
CXX = g++

# Compiler flag
CXXFLAGS = -std=c++17 -Wall -g  #-wall (warnings) -g (debugging) 
#sanitize needed later?


# Target executable
TARGET = fuzzer

# Source files
SRCS = main.cpp mutator.cpp executor.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default rule to build and run the executable
all: $(TARGET) run

# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Rule to run the executable
run: $(TARGET)
	./$(TARGET)