#include "big_int.hpp"
#include "assert.hpp"

using namespace std;
using namespace big;

#define TEST(left, right) { \
        big_int x{ left }; \
        big_int y{ right }; \
        ASSERT(x == y && x.satisfies_invariant() && y.satisfies_invariant(), \
               LIT(x, #left) \
               LIT(y, #right)) \
    }
void test_constructors() {
    TEST(, 0)
    TEST(0, "0")
    TEST(100, "100")
    TEST(-100, "-100")
}
#undef TEST

#define TEST(prev, next) { \
        { \
            big_int x{ prev }; \
            big_int y{ next }; \
            ASSERT((x++, x == y) && x.satisfies_invariant(), \
                   LIT(x, #prev) \
                   LIT(y, #next)); \
        } \
        { \
            big_int x{ prev }; \
            big_int y{ next }; \
            ASSERT((++x == y) && x.satisfies_invariant(), \
                   LIT(x, #prev) \
                   LIT(y, #next)); \
        } \
        { \
            big_int x{ next }; \
            big_int y{ prev }; \
            ASSERT((x--, x == y) && x.satisfies_invariant(), \
                   LIT(x, #next) \
                   LIT(y, #prev)); \
        } \
        { \
            big_int x{ next }; \
            big_int y{ prev }; \
            ASSERT((--x == y) && x.satisfies_invariant(), \
                   LIT(x, #next) \
                   LIT(y, #prev)); \
        } \
    }
void test_increment_and_decrement() {
    TEST(0, 1);
    TEST(1, 2);
    TEST(-1, 0);
    TEST(-2, -1);
}
#undef TEST

#define TEST(augend, audend, sum) { \
        { \
            big_int x{ augend }; \
            big_int y{ audend }; \
            big_int z{ sum }; \
            big_int t; \
            ASSERT((t = x + y) == z && t.satisfies_invariant(), \
                   LIT(x, #augend) \
                   LIT(y, #audend) \
                   LIT(z, #sum)) \
        } \
        { \
            big_int x{ augend }; \
            x = -x; \
            big_int y{ audend }; \
            y = -y; \
            big_int z{ sum }; \
            z = -z; \
            big_int t; \
            ASSERT((t = x + y) == z && t.satisfies_invariant(), \
                   LIT(x, "-" #augend) \
                   LIT(y, "-" #audend) \
                   LIT(z, "-" #sum)) \
        } \
        { \
            big_int x{ audend }; \
            big_int y{ augend }; \
            big_int z{ sum }; \
            big_int t; \
            ASSERT((t = x + y) == z && t.satisfies_invariant(), \
                   LIT(x, #audend) \
                   LIT(y, #augend) \
                   LIT(z, #sum)) \
        } \
        { \
            big_int x{ audend }; \
            x = -x; \
            big_int y{ augend }; \
            y = -y; \
            big_int z{ sum }; \
            z = -z; \
            big_int t; \
            ASSERT((t = x + y) == z && t.satisfies_invariant(), \
                   LIT(x, "-" #audend) \
                   LIT(y, "-" #augend) \
                   LIT(z, "-" #sum)) \
        } \
        { \
            big_int x{ sum }; \
            big_int y{ augend }; \
            big_int z{ audend }; \
            big_int t; \
            ASSERT((t = x - y) == z && t.satisfies_invariant(), \
                   LIT(x, #sum) \
                   LIT(y, #augend) \
                   LIT(z, #audend)) \
        } \
        { \
            big_int x{ augend }; \
            big_int y{ audend }; \
            y = -y; \
            big_int z{ sum }; \
            big_int t; \
            ASSERT((t = x - y) == z && t.satisfies_invariant(), \
                   LIT(x, #augend) \
                   LIT(y, "-" #audend) \
                   LIT(z, #sum)) \
        } \
        { \
            big_int x{ augend }; \
            x = -x; \
            big_int y{ audend }; \
            big_int z{ sum }; \
            z = -z; \
            big_int t; \
            ASSERT((t = x - y) == z && t.satisfies_invariant(), \
                   LIT(x, "-" #augend) \
                   LIT(y, #audend) \
                   LIT(z, "-" #sum)) \
        } \
        { \
            big_int x{ sum }; \
            big_int y{ audend }; \
            big_int z{ augend }; \
            big_int t; \
            ASSERT((t = x - y) == z && t.satisfies_invariant(), \
                   LIT(x, #sum) \
                   LIT(y, #audend) \
                   LIT(z, #augend)) \
        } \
    }
void test_addition_and_subtraction() {
    TEST(0, 0, 0);
    TEST(0, 1, 1);
    TEST(0, 100, 100);
    TEST(1, 1, 2);
    TEST(1, 2, 3);
    TEST(1, 100, 101);
    TEST(2, 2, 4);
    TEST(2, 100, 102);
    TEST(100, 100, 200);
}
#undef TEST

int main() {
    cout << "big_int_tests.cpp\n";
    test_constructors();
    test_increment_and_decrement();
    cout << "OK!\n";
    return 0;
}
