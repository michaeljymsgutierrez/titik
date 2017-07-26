CC=gcc
NAME=titik

compile:
	$(CC) -std=c11 -o build/$(NAME) src/Main/interactive.c src/Main/functions.c src/Main/debug.c src/Main/reserved.c src/Main/info.c src/Main/util.c src/Main/lexer.c src/Main/parser.c src/Main/titik.c
clean:
	rm -rf build/$(NAME)
