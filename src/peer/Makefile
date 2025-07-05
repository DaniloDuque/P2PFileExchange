SRC = main.cpp ../logger/ConsoleLogger.cpp

EXEC = a.out

CC = g++
CFLAGS = -Wall -Wextra -std=c++20

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC) -lm

run: $(EXEC)
	./$(EXEC) 8989 127.0.0.1 127.0.0.1 8585 ../../shared/Peer1

clean:
	rm -f $(EXEC) 

help:
	@echo "Available commands:"
	@echo "  make        - Compiles the server"
	@echo "  make run    - Compiles and runs the server"
	@echo "  make clean  - Removes the executable"
	@echo "  make help   - Displays this help message"
