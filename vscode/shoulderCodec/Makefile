Library: src/ShoulderCodec.cpp
	gcc -c src/ShoulderCodec.cpp -o obj/ShoulderCodec.o
	ar rcs obj/libShoulderCodec.a obj/ShoulderCodec.o

Test: Library test/ShoulderCodecTest.cpp main.cpp
	gcc -c test/main.cpp -o obj/main.o
	gcc obj/main.o -Lobj -lShoulderCodec -o bin/test_shoulderCodec

CleanLibrary:
	rm -f obj/ShoulderCodec.o obj/libShoulderCodec.a

CleanTest:
	rm -f obj/main.o bin/test_shoulderCodec

clean: CleanLibrary CleanTest