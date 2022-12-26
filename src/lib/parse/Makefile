all: parser_test

parser_test: parser_test.cpp Lexer.cpp
	g++ -std=c++20 -g -I../../include -o $@ $^

clean:
	rm -rf parser_test parser_test.dSYM