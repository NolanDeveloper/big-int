SOURCES := $(wildcard lib/src/*.cpp)

CXX := clang++
CXXFLAGS := \
	-std=c++14 \
	-g \
	-Wall \
	-Wextra \
	-pedantic \
	-O2 \
	-I./lib/include

.PHONY: all
all: build/libbig_int.a performance_test/build/a.out tests

.PHONY: clean
clean: 
	rm -rf build/*
	rm -rf test/build/*
	rm -rf performance_test/build/*

include lib/Makefile.mk
include test/Makefile.mk
include performance_test/Makefile.mk
