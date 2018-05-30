CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv) -liconv -lzxing

all: webcam_test

webcam_test: CvMatBitmapSource.o main.o
	g++ $(CFLAGS) $(LDFLAGS) -o webcam_test CvMatBitmapSource.o main.o

CvMatBitmapSource.o: CvMatBitmapSource.cpp CvMatBitmapSource.h
	g++ -c CvMatBitmapSource.cpp

main.o: main.cpp CvMatBitmapSource.o
	g++ -c main.cpp

clean:
	rm webcam_test *.o
