LIB_DEPS := $(patsubst lib/src/%.cpp, build/%.d, $(SOURCES))
-include $(LIB_DEPS)

BIG_INT_OBJS := $(patsubst lib/src/%.cpp, build/%.o, $(SOURCES))

build/libbig_int.a: $(BIG_INT_OBJS)
	$(AR) -rcs $@ $^

build/%.o: lib/src/%.cpp
	$(CXX) $(CXX_FLAGS) $(INC) $(LDLIBS) -c $< -o $@
	$(CXX) $(CXX_FLAGS) $(INC) -MM $< | \
		sed 's/\(.*\.o:\)/build\/\1/g' > build/$*.d 
