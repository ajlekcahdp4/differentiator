CC = gcc
CFLAGS = -Wall -Werror -Wextra
DEBUG = -g
SRC_PATH = sources/
TEMP_P = temps/

all: CONFIG_T MAIN_T LEXER_T
	$(CC) $(CFLAGS) $(DEBUG) $(TEMP_P)main.o $(TEMP_P)lexer.o -o differentiate
	rm -rf temps
CONFIG_T:
	chmod +x $(SRC_PATH)scripts/mkdir.sh
	./$(SRC_PATH)scripts/mkdir.sh temps
MAIN_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)main.c -o $(TEMP_P)main.o
LEXER_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)lexer/lexer.c -o $(TEMP_P)lexer.o
clean:
	rm -rf *.out *.o *.dat *.res
