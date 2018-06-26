CXX=clang++
CXX_FLAGS=-std=c++14 -O2 -g $(shell pkg-config --cflags opencv)
LINKER_FLAGS=-Llib -lShoulderCodec $(shell pkg-config --libs opencv)

Library: src/GeometryUtils.cpp src/ShoulderCodec.cpp src/ShoulderDetector.cpp
	$(CXX) $(CXX_FLAGS) -c src/GeometryUtils.cpp -o obj/GeometryUtils.o
	$(CXX) $(CXX_FLAGS) -c src/ShoulderCodec.cpp -o obj/ShoulderCodec.o
	$(CXX) $(CXX_FLAGS) -c src/ShoulderDetector.cpp -o obj/ShoulderDetector.o
	ar rcs lib/libShoulderCodec.a obj/GeometryUtils.o obj/ShoulderCodec.o obj/ShoulderDetector.o

Test: Library test/ShoulderCodecTest.cpp test/main.cpp
	$(CXX) $(CXX_FLAGS) -Isrc -c test/GeometryUtilsTest.cpp -o obj/GeometryUtilsTest.o
	$(CXX) $(CXX_FLAGS) -Isrc -c test/ShoulderCodecTest.cpp -o obj/ShoulderCodecTest.o
	$(CXX) $(CXX_FLAGS) -Isrc -c test/main.cpp -o obj/main.o
	$(CXX) obj/GeometryUtilsTest.o obj/ShoulderCodecTest.o obj/main.o $(LINKER_FLAGS) -o bin/test_shoulderCodec
	./bin/test_shoulderCodec

clean:
	rm -f obj/*.o lib/libShoulderCodec.a bin/test_shoulderCodec