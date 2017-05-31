CC=gcc
NAME=titik

compile:
	rm -rf src/Main/info.c
	cp src/Main/info_nix.c src/Main/info.c 
	$(CC) -std=c11 -o build/$(NAME) src/Main/functions.c src/Main/debug.c src/Main/reserved.c src/Main/info.c src/Main/util.c src/Main/lexer.c src/Main/parser.c src/Main/titik.c
clean:
	rm -rf build/$(NAME)
