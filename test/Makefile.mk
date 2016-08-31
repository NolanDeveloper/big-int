TEST_SRCS := $(wildcard test/src/*.cpp)

TEST_DEPS := $(patsubst test/src/%.cpp, test/build/%.d, $(TEST_SRCS))
-include $(TEST_DEPS)

RUN_TESTS := $(patsubst test/src/%.cpp, run_%, $(TEST_SRCS))
.PHONY: run_tests
run_tests: $(RUN_TESTS)

TESTS := $(patsubst test/src/%.cpp, test/build/%, $(TEST_SRCS))
tests: $(TESTS)

.PHONY: $(RUN_TESTS)
$(RUN_TESTS): run_%: test/build/%
	./$<

test/build/%: test/build/%.o build/libbig_int.a
	$(CXX) $< -o $@ $(CXXFLAGS) -Lbuild -lbig_int

test/build/%.o: test/src/%.cpp
	$(CXX) $(CXXFLAGS) -I./test/include -c $< -o $@
	$(CXX) $(CXXFLAGS) -I./test/include -MM $< | \
		sed 's/\(.*\.o:\)/test\/build\/\1/g' > test/build/$*.d 
