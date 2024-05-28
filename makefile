# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11 -Wall

# SFML libraries
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
SRCS = main.cpp

# Executable name
EXEC = golf-game

# Object files
OBJS = $(SRCS:.cpp=.o)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(SFML_LIBS)

# Rule to compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(EXEC)
