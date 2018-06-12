CXX=clang++
CXX_FLAGS=-std=c++14 -O2 -g

Library: src/ShoulderCodec.cpp
	$(CXX) $(CXX_FLAGS) -c src/ShoulderCodec.cpp -o obj/ShoulderCodec.o
	ar rcs obj/libShoulderCodec.a obj/ShoulderCodec.o

Test: Library test/ShoulderCodecTest.cpp test/main.cpp
	$(CXX) $(CXX_FLAGS) -Isrc -c test/ShoulderCodecTest.cpp -o obj/ShoulderCodecTest.o
	$(CXX) $(CXX_FLAGS) -Isrc -c test/main.cpp -o obj/main.o
	g++ obj/ShoulderCodecTest.o obj/main.o -Lobj -lShoulderCodec -o bin/test_shoulderCodec
	./bin/test_shoulderCodec

CleanLibrary:
	rm -f obj/ShoulderCodec.o obj/libShoulderCodec.a

CleanTest:
	rm -f obj/ShoulderCodecTest.o obj/main.o bin/test_shoulderCodec

clean: CleanLibrary CleanTest