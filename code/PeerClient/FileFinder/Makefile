SRC = FileFinder.cpp

EXEC = a.out

CC = g++
CFLAGS = -Wall -Wextra

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC) -lm

run: $(EXEC)
	./$(EXEC) 8484 192.168.100.8 8585 . saved 

clean:
	rm -f $(EXEC)

help:
	@echo "Available commands:"
	@echo "  make        - Compiles the server"
	@echo "  make run    - Compiles and runs the server"
	@echo "  make clean  - Removes the executable"
	@echo "  make help   - Displays this help message"
