#pragma once

#include <iostream>

#define ASSERT(cond, msg) \
    if (!(cond)) { \
        cerr << __FILE__ << ':' << __LINE__ << '\n'; \
        cerr << "Assertion failed: [" << #cond << "]\n"; \
        cerr << msg << '\n'; \
        exit(0); \
    }

#define B(...) __VA_ARGS__

#define LIT(x, val) "\t" #x " is " val "\n"

