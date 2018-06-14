# Attempt to load a config.make file.
# If none is found, project defaults in config.project.make will be used.
ifneq ($(wildcard config.make),)
	include config.make
endif

# make sure the the OF_ROOT location is defined
ifndef OF_ROOT
    OF_ROOT=$(realpath ../../../../../lib/of_v0.10.0_osx_release)
endif

# call the project makefile!
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk

CXX=clang++
CXX_FLAGS=-std=c++14 -O2 -g

Library: src/StringUtils.cpp
		$(CXX) $(CXX_FLAGS) -c src/StringUtils.cpp -o obj/StringUtils.o
		ar rcs lib/libStringUtils.a obj/StringUtils.o

Test: Library test/StringUtilsTest.cpp test/main.cpp
		$(CXX) $(CXX_FLAGS) -Isrc -c test/StringUtilsTest.cpp -o obj/StringUtilsTest.o
		$(CXX) $(CXX_FLAGS) -Isrc -c test/main.cpp -o obj/main.o
		g++ obj/StringUtilsTest.o obj/main.o -Llib -lStringUtils -o bin/test_stringUtils
		./bin/test_stringUtils