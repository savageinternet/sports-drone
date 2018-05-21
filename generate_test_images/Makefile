CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv) -lboost_program_options

all: generate_test_images

generate_test_images: main.cpp
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o generate_test_images

clean:
	rm generate_test_images
