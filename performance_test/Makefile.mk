PERF_SOURSES = $(wildcard performance_test/src/*)
PERF_OBJECTS = $(patsubst performance_test/src/%.cpp, \
			   performance_test/build/%.o, $(PERF_SOURSES))

performance_test/build/a.out: $(PERF_OBJECTS) ./build/libbig_int.a
	$(CXX) $(PERF_OBJECTS) -o $@ $(CXXFLAGS) -L./build -lbig_int

performance_test/build/%.o: performance_test/src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)
	$(CXX) $(CXXFLAGS) -MM $< \
		| sed 's/\(.*\.o:\)/performance_test\/build\/\1/g' > build/$*.d 

-include $(wildcard performance_test/build/*.d)
