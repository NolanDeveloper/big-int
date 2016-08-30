SOURSES = $(wildcard src/*)
OBJECTS = $(patsubst src/%.cpp, build/%.o, $(SOURSES))

CXX = clang++
CXXFLAGS = -std=c++14 -g -Iinclude -I../lib/include -O2
LDLIBS = -L../build/ -lbig_int

build/a.out: $(OBJECTS)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDLIBS)

build/%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)
	$(CXX) $(CXXFLAGS) -MM $< | sed 's/\(.*\.o:\)/build\/\1/g' > build/$*.d 

clean:
	@rm ./build/*

-include $(wildcard build/*.d)
