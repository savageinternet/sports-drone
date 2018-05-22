CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv zbar)

all: main bgsub pplfind

build/main:
	mkdir -p build
	rm -f build/*
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o build/main

clean:
	rm -r build

bgsub:
	g++ $(CFLAGS) $(LDFLAGS) bgsub.cpp -o build/bgsub

pplfind:
	g++ $(CFLAGS) $(LDFLAGS) pplfind.cpp -o build/pplfind

blobdetect:
	g++ $(CFLAGS) $(LDFLAGS) blobdetect.cpp -o build/blobdetect
