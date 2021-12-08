# Makefile
# Author: Peter Rucek, xrucek00
# Date: 8.12.2021


all: clean algo

algo: 
	g++ -std=c++11 -Wall -Wextra -Werror -pedantic algo.cpp -o algo 

run: clean algo
	./algo

.PHONY: clean

clean:
	rm -f algo