CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv zbar)

all: main bgsub

build/main:
	mkdir -p build
	rm -f build/*
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o build/main

clean:
	rm -r build

bgsub:
	g++ $(CFLAGS) $(LDFLAGS) bgsub.cpp -o build/bgsub
