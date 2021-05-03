CC = clang++

CFLAGS = -Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow
CFLAGS += -Wunreachable-code -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual
CFLAGS += -std=c++17 -g

all: acram

acram: common.o expr_tree.o parser.o texio.o main.o
	$(CC) common.o expr_tree.o parser.o texio.o main.o -o acram

common.o: common.cpp common.hpp
	$(CC) $(CFLAGS) -c common.cpp

expr_tree.o: expr_tree.cpp expr_tree.hpp
	$(CC) $(CFLAGS) -c expr_tree.cpp

parser.o: parser.cpp parser.hpp
	$(CC) $(CFLAGS) -c parser.cpp

texio.o: texio.cpp texio.hpp
	$(CC) $(CFLAGS) -c texio.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp
	