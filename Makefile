CC = gcc
CFLAGS = -Wall -Werror -Wextra
DEBUG = -g
SRC_PATH = sources/
TEMP_P = temps/

all: CONFIG_T MAIN_T LEXER_T PARSER_T DUMP_T DERIVATE_T WRITETEX_T
	$(CC) $(CFLAGS) $(DEBUG) $(TEMP_P)main.o $(TEMP_P)lexer.o $(TEMP_P)parser.o $(TEMP_P)dump_tree.o $(TEMP_P)derivatives.o $(TEMP_P)writetex.o -o differentiate
	rm -rf temps
CONFIG_T:
	chmod +x $(SRC_PATH)scripts/mkdir.sh
	./$(SRC_PATH)scripts/mkdir.sh temps
MAIN_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)main.c -o $(TEMP_P)main.o
LEXER_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)lexer/lexer.c -o $(TEMP_P)lexer.o
PARSER_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)parser/parser.c -o $(TEMP_P)parser.o
DUMP_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)dump_tree/dump_tree.c -o $(TEMP_P)dump_tree.o
DERIVATE_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)derivatives/derivatives.c -o $(TEMP_P)derivatives.o
WRITETEX_T:
	$(CC) $(CFLAGS) -c $(DEBUG) $(SRC_PATH)writetex/writetex.c -o $(TEMP_P)writetex.o
clean:
	rm -rf *.out *.o *.dat *.res
