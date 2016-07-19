TEST_SRCS := $(wildcard test/src/*.cpp)

TEST_DEPS := $(patsubst test/src/%.cpp, test/build/%.d, $(TEST_SRCS))
-include $(TEST_DEPS)

RUN_TESTS := $(patsubst test/src/%.cpp, run_%, $(TEST_SRCS))
.PHONY: test-all
test-all: test_build_dir $(RUN_TESTS)

.PHONY: test_build_dir
test_build_dir:
	mkdir -p test/build

.PHONY: $(RUN_TESTS)
$(RUN_TESTS): run_%: test/build/%
	./$<

test/build/%: test/build/%.o build/libbig_int.a
	$(CXX) $(CXX_FLAGS) $< -Lbuild -lbig_int -o $@

test/build/%.o: test/src/%.cpp
	$(CXX) $(CXX_FLAGS) $(INC) -c $< -o $@
	$(CXX) $(CXX_FLAGS) $(INC) -MM $< | \
		sed 's/\(.*\.o:\)/test\/build\/\1/g' > test/build/$*.d 
