# Makefile
# Author: Peter Rucek, xrucek00
# Date: 8.12.2021


all: clean simulation

simulation: 
	g++ -std=c++11 -Wall -Wextra -Werror -pedantic simulation.cpp -o simulation -O3

run: clean simulation
	./simulation

.PHONY: clean

clean:
	rm -f simulation