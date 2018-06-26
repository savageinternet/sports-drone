CXX=clang++
CXX_FLAGS=-std=c++14 -O2 -g $(shell pkg-config --cflags opencv)
LINKER_FLAGS=-Llib -lShoulderCodec $(shell pkg-config --libs opencv)

Library: src/ShoulderCodec.cpp src/ShoulderDetector.cpp
	$(CXX) $(CXX_FLAGS) -c src/ShoulderCodec.cpp -o obj/ShoulderCodec.o
	$(CXX) $(CXX_FLAGS) -c src/ShoulderDetector.cpp -o obj/ShoulderDetector.o
	ar rcs lib/libShoulderCodec.a obj/ShoulderCodec.o obj/ShoulderDetector.o

Test: Library test/ShoulderCodecTest.cpp test/main.cpp
	$(CXX) $(CXX_FLAGS) -Isrc -c test/ShoulderCodecTest.cpp -o obj/ShoulderCodecTest.o
	$(CXX) $(CXX_FLAGS) -Isrc -c test/main.cpp -o obj/main.o
	$(CXX) obj/ShoulderCodecTest.o obj/main.o $(LINKER_FLAGS) -o bin/test_shoulderCodec
	./bin/test_shoulderCodec

CleanLibrary:
	rm -f obj/ShoulderCodec.o obj/ShoulderDetector.o lib/libShoulderCodec.a

CleanTest:
	rm -f obj/ShoulderCodecTest.o obj/main.o bin/test_shoulderCodec

clean: CleanLibrary CleanTest