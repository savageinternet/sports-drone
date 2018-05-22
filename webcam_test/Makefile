CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv) -lboost_program_options

all: webcam_test

webcam_test: main.cpp
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o webcam_test

clean:
	rm webcam_test
