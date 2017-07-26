del /f src\Main\info.c

copy src\Main\info_win.c src\Main\info.c

cl /EHsc src/Main/interactive.c src/Main/functions.c src/Main/debug.c src/Main/reserved.c src/Main/info.c src/Main/util.c src/Main/lexer.c src/Main/parser.c src/Main/titik.c /link /out:build/titik.exe