SOURCES := $(wildcard lib/src/*.cpp)

CXX := clang++
CXX_FLAGS := -std=c++14 -g
INC := -I./lib/include -I./test/include

.PHONY: all
all: build_dir build/libbig_int.a

.PHONY: build_dir
build_dir:
	mkdir -p build

.PHONY: clean
clean: 
	rm -rf build/*
	rm -rf test/build/*

include lib/Makefile.mk
include test/Makefile.mk
