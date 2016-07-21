#include "big_uint.hpp"

#include <tuple>
#include <utility>
#include <iterator>
#include <limits>
#include <vector>
#include <sstream>

using namespace big;
using namespace std;

#define PRINT_FILE_AND_LINE \
    cerr << __FILE__ << ':' << __LINE__ << '\n';

#define ASSERT(cond, msg) \
    if (!(cond)) { \
        PRINT_FILE_AND_LINE \
        cerr << "Assertion failed: [" << #cond << "]\n"; \
        cerr << msg << '\n'; \
        exit(0); \
    }

#define ASSERT_THROWS(expr, msg) \
    try { \
        expr; \
        PRINT_FILE_AND_LINE \
        cerr << "Expression didn't throw exception: [" << #expr << "]\n"; \
        cerr << msg << '\n'; \
    } catch (...) { }

#define B(...) __VA_ARGS__

#define LIT(x, val) "\t" #x " is " val "\n"

static const digit      m  = numeric_limits<digit>::max();
static const long_digit mm = numeric_limits<long_digit>::max();

#define TEST(init, content) { \
        big_uint     x{ init }; \
        deque<digit> y = { content }; \
        ASSERT(x.digits() == y && x.satisfies_invariant(), \
               LIT(x, #init) \
               LIT(y, #content)) \
    }
void test_constructors() {
    deque<digit> value;
    string num = "hello 5352353554325545235435 world!";
    TEST(, B(0))
    TEST(B(1), B(1))
    TEST(B(432), B(432))
    TEST("0", B(0))
    TEST("123", B(123))
    TEST("100", B(100))
    TEST("999", B(999))
    TEST("256", B(256))
    TEST("257", B(257))
    TEST("255", B(255))
    TEST(to_string(m), B(m))
    TEST(to_string((long_digit) m + 1), B(0, 1))
    TEST(to_string(mm), B(m, m))
    TEST("340282367000166625996085689103316680705", B(1, 1, 1, 1, 1))
    TEST("339942088881935131302157980349511352979684", 
         B(4324, 5453, 43262, 54626, 999))
    TEST("00000000001024", B(1024))
    TEST("18446744090889420804", B(4, 4, 1))
    TEST(B(num.begin() + 6, num.end() - 7), B(2978821099, 651407276, 290))
}
#undef TEST

#define TEST(prev, next) { \
        { \
            big_uint x{ next }; \
            big_uint y{ prev }; \
            ASSERT(--x == y && x.satisfies_invariant(), \
                   LIT(x, #next) \
                   LIT(y, #prev)) \
        } \
        { \
            big_uint x{ next }; \
            big_uint y{ prev }; \
            ASSERT((x--, x == y && x.satisfies_invariant()), \
                   LIT(x, #next) \
                   LIT(y, #prev)) \
        } \
        { \
            big_uint x{ prev }; \
            big_uint y{ next }; \
            ASSERT(++x == y && x.satisfies_invariant(), \
                   LIT(x, #prev) \
                   LIT(y, #next)) \
        } \
        { \
            big_uint x{ prev }; \
            big_uint y{ next }; \
            ASSERT((x++, x == y && x.satisfies_invariant()), \
                   LIT(x, #prev) \
                   LIT(y, #next)) \
        } \
    }
void test_increment_and_decrement() {
    TEST(B(0), B(1))
    TEST(B(41), B(42))
    TEST(B(42), B(43))
    TEST(B(127), B(128))
    TEST(B(128), B(129))
    TEST(B(m), B(0, 1))
    TEST(B(m, m), B(0, 0, 1))
    TEST(B(0, 0, 1), B(1, 0, 1))
    TEST(B(m, 0, 1), B(0, 1, 1))
    TEST(B(42, 42), B(43, 42))
}
#undef TEST

#define TEST(augend, audend, sum) { \
        big_uint x{ augend }; \
        digit    y{ audend }; \
        big_uint z{ sum }; \
        big_uint t; \
        ASSERT((t = x + y) == z && t.satisfies_invariant(), \
               LIT(x, #augend) \
               LIT(y, #audend) \
               LIT(z, #sum)) \
        ASSERT((t = y + x) == z && t.satisfies_invariant(), \
               LIT(x, #augend) \
               LIT(y, #audend) \
               LIT(z, #sum)) \
        ASSERT((x += y) == z && x.satisfies_invariant(), \
               LIT(x, #augend) \
               LIT(y, #audend) \
               LIT(z, #sum)) \
    }
void test_add_digit() {
    TEST(B(0), 0, B(0));
    TEST(B(0), 1, B(1));
    TEST(B(1), 0, B(1));
    TEST(B(1), 1, B(2));
    TEST(B(m), 0, B(m));
    TEST(B(0), m, B(m));
    TEST(B(m), 1, B(0, 1));
    TEST(B(1), m, B(0, 1));
    TEST(B(m - 41), 42, B(0, 1));
    TEST(B(42), m - 41, B(0, 1));
    TEST(B(m), 42, B(41, 1));
    TEST(B(42), m, B(41, 1));
    TEST(B(m), m, B(m - 1, 1));
    TEST(B(0, 1), 1, B(1, 1));
    TEST(B(1, 1), 1, B(2, 1));
    TEST(B(m, 42), 1, B(0, 43));
    TEST(B(m, 42), 42, B(41, 43));
    TEST(B(m - 41, 42), 42, B(0, 43));
    TEST(B(m, m), 1, B(0, 0, 1));
    TEST(B(m, m, m), 1, B(0, 0, 0, 1));
    TEST(B(m, m, m), 0, B(m, m, m));
    TEST(B(m, m, m), 100, B(99, 0, 0, 1));
    TEST(B(m, m, 5), 100, B(99, 0, 6));
    TEST("3243267419327147878943294639276", 0, "3243267419327147878943294639276");
}
#undef TEST

#define TEST(minuend, subtrahend, difference) { \
        big_uint x{ minuend }; \
        digit    y{ subtrahend }; \
        big_uint z{ difference }; \
        big_uint t; \
        ASSERT((t = x - y) == z && t.satisfies_invariant(), \
               LIT(x, #minuend) \
               LIT(y, #subtrahend) \
               LIT(z, #difference)) \
        ASSERT((x -= y) == z && x.satisfies_invariant(), \
               LIT(x, #minuend) \
               LIT(y, #subtrahend) \
               LIT(z, #difference)) \
    }
void test_subtract_digit() {
    TEST(B(0), 0, B(0))
    TEST(B(1), 0, B(1))
    TEST(B(1), 1, B(0))
    TEST(B(50), 5, B(45))
    TEST(B(m), m, B(0))
    TEST(B(m), m - 1, B(1))
    TEST(B(m), 1, B(m - 1))
    TEST(B(0, 1), 42, B(m - 41))
    TEST(B(0, 1), 1, B(m))
    TEST(B(0, 42), 1, B(m, 41))
    TEST(B(0, 42), 42, B(m - 41, 41))
    TEST(B(0, 0, 0, 0, 0, 1), 1, B(m, m, m, m, m))
    TEST(B(0, 0, 0, 0, 0, 1), m, B(1, m, m, m, m))
}
#undef TEST

#define TEST(minuend, subtrahend, difference) { \
        digit    x{ minuend }; \
        big_uint y{ subtrahend }; \
        digit    z{ difference }; \
        digit    t; \
        ASSERT((t = x - y) == z, \
               LIT(x, #minuend) \
               LIT(y, #subtrahend) \
               LIT(z, #difference)) \
    }
void test_reverse_subtract_digit() {
    TEST(0, B(0), 0)
    TEST(1, B(0), 1)
    TEST(42, B(0), 42)
    TEST(m, B(0), m)
    TEST(1, B(1), 0)
    TEST(42, B(1), 41)
    TEST(m, B(1), m - 1)
    TEST(2, B(2), 0)
    TEST(42, B(2), 40)
    TEST(m, B(2), m - 2)
    TEST(m, B(m), 0)
}
#undef TEST

#define TEST(multiplier, multiplicand, product) { \
        big_uint x{ multiplier }; \
        digit    y{ multiplicand }; \
        big_uint z{ product }; \
        big_uint t; \
        ASSERT((t = x * y) == z && t.satisfies_invariant(), \
               LIT(x, #multiplier) \
               LIT(y, #multiplicand) \
               LIT(z, #product)) \
        ASSERT((x *= y) == z && x.satisfies_invariant(), \
               LIT(x, #multiplier) \
               LIT(y, #multiplicand) \
               LIT(z, #product)) \
    }
void test_multiply_digit() {
    auto l = [] (long_digit d) -> digit { return d; };
    auto h = [] (long_digit d) -> digit { return d >> (8 * sizeof(digit)); };
    TEST(B(0), 0, B(0))
    TEST(B(0), 1, B(0))
    TEST(B(0), 42, B(0))
    TEST(B(1), 0, B(0))
    TEST(B(42), 0, B(0))
    TEST(B(0, 1), 0, B(0))
    TEST(B(0, 42), 0, B(0))
    TEST(B(1), 1, B(1))
    TEST(B(42), 1, B(42))
    TEST(B(1), 42, B(42))
    TEST(B(0, 1), 42, B(0, 42))
    TEST(B(1, 1), 42, B(42, 42))
    TEST(B(2), 2, B(4))
    TEST(B(3), 2, B(6))
    TEST(B(2), 3, B(6))
    TEST(B(2), 3, B(6))
    TEST(B(2), m, B(2 * m, 1))
    TEST(B(m), 2, B(2 * m, 1))
    TEST(B(m), m, B(l((long_digit) m * m), h((long_digit) m * m)))
    TEST(B(1, m), m, B(m, l((long_digit) m * m), h((long_digit) m * m)))
    TEST(B(0, m), m, B(0, l((long_digit) m * m), h((long_digit) m * m)))
}
#undef TEST

#define TEST(dividend, divisor, quotient, reminder) { \
        { \
            big_uint x{ dividend }; \
            digit    y{ divisor }; \
            big_uint z{ quotient }; \
            big_uint t; \
            ASSERT((t = x / y) == z && t.satisfies_invariant(), \
                   LIT(x, #dividend) \
                   LIT(y, #divisor) \
                   LIT(z, #quotient)) \
            ASSERT((x /= y) == z && x.satisfies_invariant(), \
                   LIT(x, #dividend) \
                   LIT(y, #divisor) \
                   LIT(z, #quotient)) \
        } \
        { \
            big_uint x{ dividend }; \
            digit    y{ divisor }; \
            digit    z{ reminder }; \
            ASSERT((x % y) == z, \
                   LIT(x, #dividend) \
                   LIT(y, #divisor) \
                   LIT(z, #quotient)) \
            ASSERT((x %= y) == z && x.satisfies_invariant(), \
                   LIT(x, #dividend) \
                   LIT(y, #divisor) \
                   LIT(z, #reminder)) \
        } \
    }
void test_divide_digit() {
    TEST(B(0), 1, B(0), 0)
    TEST(B(0), 2, B(0), 0)
    TEST(B(0), 100, B(0), 0)
    TEST(B(0), 1, B(0), 0)
    TEST(B(1), 1, B(1), 0)
    TEST(B(1), 2, B(0), 1)
    TEST(B(2), 2, B(1), 0)
    TEST(B(4), 2, B(2), 0)
    TEST(B(m), 2, B(m / 2), 1)
    TEST(B(0, 1), 2, B(m / 2 + 1), 0)
    TEST(B(0, m), 2, B(1u << (8 * sizeof(digit) - 1), m >> 1), 0)
    TEST(B(0, m), 4, B(3u << (8 * sizeof(digit) - 2), m >> 2), 0)
    TEST("311543243254325435435441361748615345432543254325432543254325", 54354, 
         "5731744549698742234894237070843274559968783425790788962", 13777)
    TEST("43621874963271894632871946721389468932164", 2, 
         "21810937481635947316435973360694734466082", 0)
    TEST("21810937481635947316435973360694734466082", 2,
         "10905468740817973658217986680347367233041", 0);
    TEST("10905468740817973658217986680347367233041", 2,
         "5452734370408986829108993340173683616520", 1);
    TEST("5452734370408986829108993340173683616520", 2,
         "2726367185204493414554496670086841808260", 0);
    TEST("2726367185204493414554496670086841808260", 2,
         "1363183592602246707277248335043420904130", 0);
    TEST("1363183592602246707277248335043420904130", 2,
         "681591796301123353638624167521710452065", 0);
    TEST("681591796301123353638624167521710452065", 2,
         "340795898150561676819312083760855226032", 1);
    TEST("681591796301123353638624167521710452065", 43432432,
         "15693152902446801819401321287320", 32089825);
}
#undef TEST

#define TEST(dividend, divisor, quotient, reminder) { \
        digit    x{ dividend }; \
        big_uint y{ divisor }; \
        big_uint z{ quotient }; \
        digit    r{ reminder }; \
        ASSERT(x / y == z, \
               LIT(x, #dividend) \
               LIT(y, #divisor) \
               LIT(z, #quotient)) \
        ASSERT(x % y == r, \
               LIT(x, #dividend) \
               LIT(y, #divisor) \
               LIT(r, #reminder)) \
    }
void test_reverse_divide_digit() {
    TEST(0, B(1), B(0), 0)
    TEST(1, B(1), B(1), 0)
    TEST(2, B(1), B(2), 0)
    TEST(42, B(1), B(42), 0)
    TEST(m, B(1), B(m), 0)
    TEST(1, B(2), B(0), 1)
    TEST(2, B(2), B(1), 0)
    TEST(42, B(2), B(21), 0)
    TEST(43, B(2), B(21), 1)
    TEST(m, B(2), B(m >> 1), 1)
    TEST(m - 1, B(2), B(m >> 1), 0)
    TEST(0, B(m), B(0), 0)
    TEST(1, B(m), B(0), 1)
    TEST(2, B(m), B(0), 2)
    TEST(m - 1, B(m), B(0), m - 1)
    TEST(m, B(m), B(1), 0)
}
#undef TEST

#define TEST(augend, audend, sum) { \
        big_uint t; \
        { \
            big_uint x{ augend }; \
            big_uint y{ audend }; \
            big_uint z{ sum }; \
            ASSERT((t = x + y) == z && t.satisfies_invariant(), \
                   LIT(x, #augend) \
                   LIT(y, #audend) \
                   LIT(z, #sum)) \
            ASSERT((x += y) == z && x.satisfies_invariant(), \
                   LIT(x, #augend) \
                   LIT(y, #audend) \
                   LIT(z, #sum)) \
        } \
        { \
            big_uint x{ audend }; \
            big_uint y{ augend }; \
            big_uint z{ sum }; \
            ASSERT((t = x + y) == z && t.satisfies_invariant(), \
                   LIT(x, #audend) \
                   LIT(y, #augend) \
                   LIT(z, #sum)) \
            ASSERT((x += y) == z && x.satisfies_invariant(), \
                   LIT(x, #audend) \
                   LIT(y, #augend) \
                   LIT(z, #sum)) \
        } \
        { \
            big_uint x{ sum }; \
            big_uint y{ augend }; \
            big_uint z{ audend }; \
            ASSERT((t = x - y) == z && t.satisfies_invariant(), \
                   LIT(x, #sum) \
                   LIT(y, #augend) \
                   LIT(z, #audend)) \
            ASSERT((x -= y) == z && x.satisfies_invariant(), \
                   LIT(x, #sum) \
                   LIT(y, #augend) \
                   LIT(z, #audend)) \
        } \
        { \
            big_uint x{ sum }; \
            big_uint y{ audend }; \
            big_uint z{ augend }; \
            ASSERT((t = x - y) == z && t.satisfies_invariant(), \
                   LIT(x, #sum) \
                   LIT(y, #audend) \
                   LIT(z, #augend)) \
            ASSERT((x -= y) == z && x.satisfies_invariant(), \
                   LIT(x, #sum) \
                   LIT(y, #audend) \
                   LIT(z, #augend)) \
        } \
    } 
void test_add_and_subtract() {
    TEST(B(0), B(0), B(0))
    TEST(B(0), B(1), B(1))
    TEST(B(0), B(42), B(42))   
    TEST(B(0), B(0, 1), B(0, 1))   
    TEST(B(0), B(0, 42), B(0, 42))   
    TEST(B(0), B(43, 42), B(43, 42))   
    TEST(B(0), B(0, 0, 1), B(0, 0, 1))   
    TEST(B(0), B(0, 0, 42), B(0, 0, 42))   
    TEST(B(0), B(0, 43, 42), B(0, 43, 42))   
    TEST(B(0), B(44, 43, 42), B(44, 43, 42))   
    TEST(B(0), B(44, 0, 42), B(44, 0, 42))   
    TEST(B(0), B(m), B(m))   
    TEST(B(0), B(m, m), B(m, m))   
    TEST(B(0), B(m, m, 1), B(m, m, 1))   
    TEST(B(0), B(m, 0, m), B(m, 0, m))   
    TEST(B(1), B(1), B(2))
    TEST(B(1), B(41), B(42))   
    TEST(B(1), B(m), B(0, 1))
    TEST(B(1), B(m, m), B(0, 0, 1))
    TEST(B(1), B(m, m, m), B(0, 0, 0, 1))
    TEST(B(1), B(m, m, m, m), B(0, 0, 0, 0, 1))   
    TEST(B(42), B(128), B(170))
    TEST(B(42), B(m), B(41, 1))
    TEST(B(42), B(m, m), B(41, 0, 1))
    TEST(B(42), B(m, m, m), B(41, 0, 0, 1))
    TEST(B(3555), B(32134 - 3555), B(32134))   
    TEST(B(0, 1), B(0, 1), B(0, 2))
    TEST(B(0, 42), B(0, 43), B(0, 85))
    TEST(B(1, 1), B(1, 1), B(2, 2))
    TEST(B(42, 1), B(43, 1), B(85, 2))
    TEST(B(m, m, m), B(m, m, m), B(m - 1, m, m, 1))
    TEST(("5412218217130331892"), ("23213214343532432"), ("5435431431473864324"))
    TEST(("7427255953219916513991965128"), ("76353637546496645509677056"), 
         ("7503609590766413159501642184"))
    TEST(("4117099328278639779421887554"), ("373069782234956328652655603"), 
         ("4490169110513596108074543157"))
}
#undef TEST

#define TEST(multiplier, multiplicand, product) { \
        big_uint t; \
        { \
            big_uint x{ multiplier }; \
            big_uint y{ multiplicand }; \
            big_uint z{ product }; \
            ASSERT((t = x * y) == z && t.satisfies_invariant(), \
                   LIT(x, #multiplier) \
                   LIT(y, #multiplicand) \
                   LIT(z, #product)) \
            ASSERT((x *= y) == z && x.satisfies_invariant(), \
                   LIT(x, #multiplier) \
                   LIT(y, #multiplicand) \
                   LIT(z, #product)) \
        } \
        { \
            big_uint x{ multiplicand }; \
            big_uint y{ multiplier }; \
            big_uint z{ product }; \
            ASSERT((t = x * y) == z && t.satisfies_invariant(), \
                   LIT(x, #multiplicand) \
                   LIT(y, #multiplier) \
                   LIT(z, #product)) \
            ASSERT((x *= y) == z && x.satisfies_invariant(), \
                   LIT(x, #multiplicand) \
                   LIT(y, #multiplier) \
                   LIT(z, #product)) \
        } \
    }
void test_multiply() {
    TEST(B(0), B(0), B(0))
    TEST(B(0), B(1), B(0))
    TEST(B(0), B(42), B(0))
    TEST(B(0), B(0, 42), B(0))
    TEST(B(0), B(43, 42), B(0))
    TEST(B(0), B(43, 42, 41), B(0))
    TEST(B(0), B(43, 0, 41), B(0))
    TEST(B(1), B(1), B(1))
    TEST(B(1), B(42), B(42))
    TEST(B(1), B(0, 42), B(0, 42))
    TEST(B(1), B(43, 42), B(43, 42))
    TEST(B(1), B(43, 42, 41), B(43, 42, 41))
    TEST(B(1), B(43, 0, 41), B(43, 0, 41))
    TEST(B(1), B(43, 0, 41), B(43, 0, 41))
    TEST(B(2), B(1), B(2))
    TEST(B(2), B(2), B(4))
    TEST(B(2), B(m), B(2 * m, 1))
    TEST(B(4), B(m), B(4 * m, 3))
    TEST(B(8), B(m), B(8 * m, 7))
    TEST(B(m), B(m), B(1, m - 1))
    TEST(B(m), B(m, m), B(1, m, m - 1))
    TEST(B(0, 1), B(0, 1), B(0, 0, 1))
    TEST(B(m, m), B(m, m), B(1, 0, m - 1, m))
    TEST(B(m, m, m, m, m), B(m, m, m), B(1, 0, 0, m, m, m - 1, m, m))
    TEST(("32153214732946372463729643826473821964783921648"), 
         ("4324324234136748316481"), 
         ("139040925675082734651478887262280244116520657008305538586301011080688"))
    big_uint x{ 0, 1 };
    big_uint y{ 0, 0, 1 };
    big_uint t;
    ASSERT((t = x * x) == y && t.satisfies_invariant(),
           LIT(x, "B(m)")
           LIT(y, "B(1, m - 1)"))
    ASSERT((x *= x) == y && t.satisfies_invariant(),
           LIT(x, "B(m)")
           LIT(y, "B(1, m - 1)"))
}
#undef TEST

#define TEST(dividend, divisor, quotient, reminder) { \
        { \
            big_uint x{ dividend }; \
            big_uint y{ divisor }; \
            big_uint z{ quotient }; \
            ASSERT((x /= y) == z && x.satisfies_invariant(), \
                   LIT(x, #dividend) \
                   LIT(y, #divisor) \
                   LIT(z, #quotient)) \
        } \
        { \
            big_uint x{ dividend }; \
            big_uint y{ divisor }; \
            big_uint z{ reminder }; \
            ASSERT((x %= y) == z && x.satisfies_invariant(), \
                   LIT(x, #dividend) \
                   LIT(y, #divisor) \
                   LIT(z, #reminder)) \
        } \
    }
void test_divide() {
    TEST(B(0), B(1), B(0), B(0))
    TEST(B(1), B(2), B(0), B(1))
    TEST(B(100), B(101), B(0), B(100))
    TEST(B(3242), B(3242342), B(0), B(3242))
    TEST(B(m - 1), B(m), B(0), B(m - 1))
    TEST(B(1), B(m), B(0), B(1))
    TEST(B(1), B(0, 1), B(0), B(1))
    TEST(B(1), B(1, 1), B(0), B(1))
    TEST(B(0, 1), B(1, 1), B(0), B(0, 1))
    TEST(B(0, 1), B(m, 1), B(0), B(0, 1))
    TEST(B(m - 1, m), B(m, m), B(0), B(m - 1, m))
    TEST(B(m - 1, m), B(m, m, m, m, m), B(0), B(m - 1, m))
    TEST(B(1), B(1), B(1), B(0))
    TEST(B(m), B(m), B(1), B(0))
    TEST(B(100), B(100), B(1), B(0))
    TEST(B(0, 1), B(0, 1), B(1), B(0))
    TEST(B(m, m, m), B(m, m, m), B(1), B(0))
    TEST(B(2), B(1), B(2), B(0))
    TEST(B(3), B(1), B(3), B(0))
    TEST(B(42), B(1), B(42), B(0))
    TEST(B(m), B(1), B(m), B(0))
    TEST(B(m, m), B(1), B(m, m), B(0))
    TEST(B(m, m, m, m), B(1), B(m, m, m, m), B(0))
    TEST(B(4), B(2), B(2), B(0))
    TEST(B(12), B(4), B(3), B(0))
    TEST(B(40), B(10), B(4), B(0))
    TEST(B(m), B(2), B(m / 2), B(1))
    TEST(B(m), B(3), B(m / 3), B(m % 3))
    TEST(B(m), B(42), B(m / 42), B(m % 42))
    TEST(B(m, m), B(2), B(m, m / 2), B(1))
    TEST(B(m, m), B(4), B(m, m / 4), B(3))
    TEST(B(m, m), B(8), B(m, m / 8), B(7))
    TEST(B(m, m), B(16), B(m, m / 16), B(15))
    TEST(B(m, m), B(m), B(1, 1), B(0))
    TEST(B(m, m, m, m), B(m, m), B(1, 0, 1), B(0))
    TEST(B(64), B(2), B(32), B(0))
    TEST(B(0, 64), B(2), B(0, 32), B(0))
    TEST(B(0, 64), B(m), B(64), B(64))
    TEST(B(m, m, m, m, 64), B(m, m), B(65, 0, 65), B(64))
    TEST(("193337807559688298930754147171641093868975707"),
         ("4490169110513596108074543157"),
         ("43058023606949152"),
         ("1933748539936698160940422843"))
    TEST(("75528846334302370804827124168427154062500000"),
         ("1683636112060604732504683448"),
         ("44860552582150607"),
         ("447653616291209758466447064"))
    TEST(("1132525508412777091109288731362817259853788043"),
         ("2463055200723804344752501952"),
         ("459805167208582297"),
         ("2090271824848574522208644299"))
    TEST(("295006136719563272045078555355314325000044451"),
         ("1340168791122082979216216000"),
         ("220126105512846270"),
         ("646047603915110425885724451"))
    TEST(("35756746887336380751538542246333586104625357824"),
         ("7616464909621040695721928983"),
         ("4694664429185359106"),
         ("843662872430705616740988626"))
    TEST(("4741878768312026933022886262165697667042658701"),
         ("3654992685691111205162875483"),
         ("1297370248338923780"),
         ("478364294338710608674972961"))
    TEST(("2600188226511891395478053392718237397137093"),
         ("2214567202119851126722000553"),
         ("1174129294438620"),
         ("1822481037247953933932580233"))
    TEST(("52560992936204269321896054508110584832"),
         ("141906137908840773415628409"),
         ("370392667369"),
         ("115567489776503227266898911"))
    TEST(("2222898338643105118962354568339698257133568"),
         ("5150437746469652216012707000"),
         ("431594060168338"),
         ("2862900225027880131186167568"))
    TEST(("4312255903554341144066849577228089065789143"),
         ("638325266344800352689264291"),
         ("6755577651261285"),
         ("604165436825163714204515208"))
    TEST(("1682143229012189349805232082310945588290543"),
         ("12757973275316991201026624"),
         ("131850349010109048"),
         ("10097045870352413796996591"))
    TEST(("45426857196788382413931916718693308253171053568"),
         ("102965937463351835666579456"),
         ("441183349716569543355"),
         ("19233014713321432426738688"))
    TEST(("1621623272887369323359690431656860474019300000"),
         ("5851264858867636842717814627"),
         ("277140637452052233"),
         ("4758661388498505184003887909"))
    TEST(("3941626601019122942408291156204078388197406343"),
         ("256923537735125398888494379"),
         ("15341632906684991388"),
         ("61136043202959592295998291"))
    TEST(("4810921854995887810714597765554820688330700000"),
         ("1427241079981484551656197632"),
         ("3370784321215235336"),
         ("285354503877361762524775648"))
    TEST(("1337981899422943908907000637111098478076264224"),
         ("9116472605804618828260289993"),
         ("146765306854652011"),
         ("4586689973978595146115638301"))
    TEST(("97814509989165608223697727684258329184248832"),
         ("4545242415007334558919545507"),
         ("21520196517176909"),
         ("4228987060631810032189150969"))
    TEST(("1852044222687172754512244972653265714434300000"),
         ("4519766203612463069074069624"),
         ("409765492119240603"),
         ("143507974061027992804556728"))
    TEST(("469018580489436804052981776271238180425985632"),
         ("7017730422423651698263488000"),
         ("66833370941521004"),
         ("230149064981333902124033632"))
    TEST(("83367202253609923934141183321621299392365625"),
         ("433623688284936030965856872"),
         ("192257029553304682"),
         ("211429884667762421972890921"))
    TEST(("15702386492996888950308262150822441066202475999"),
         ("7687297046420938881733078125"),
         ("2042640787545425367"),
         ("6863950899586122958834679124"))
    TEST(("75428358360098820558835293615837802392576"),
         ("8472999854175697904553936811"),
         ("8902202249292"),
         ("5134674351346207019964904764"))
    TEST(("1838903138206502729914814698293531335217049824"),
         ("6311473551050869632524183"),
         ("291358764848237802052"),
         ("4713467368213414560026308"))
    TEST(("11983196336932085645125131770676927699295232"),
         ("2889189632750754142906320141"),
         ("4147597721207058"),
         ("973997464386047032202540054"))
    TEST(("25536819334700733676710684837746486802851791557"),
         ("420596955138084987703975624"),
         ("60715654316415137377"),
         ("316779037473587925832493309"))
    TEST(("49699753337881641431263771469436134910543"),
         ("79502401625653519277211"),
         ("625135245245782894"),
         ("21288823458503427081909"))
    TEST(("917407177119868685051081742909017192847334757"),
         ("2487664168524508426709768"),
         ("368782566685439801374"),
         ("1774643949396578581713525"))
    TEST(("35082204086211562347881877456499751577311776"),
         ("9056414127365363740365782776"),
         ("3873741151059471"),
         ("3610175119377778242933840280"))
    TEST(("27301271478031964520914855746528826777302678125"),
         ("6101764819983846018047959552"),
         ("4474323787212802257"),
         ("1241877880020294826392369261"))
    TEST(("61586499853823005267722354972735539669585457"),
         ("1557372533965684033851162216"),
         ("39545130346558452"),
         ("51688864597353083691735825"))
    TEST(("339628286150170618468395797227784036124019007"),
         ("4351677652590841046371717000"),
         ("78045368536883120"),
         ("217563254770815933406979007"))
    TEST(("1615356973582482194670478770106867527556641824"),
         ("2948357140608186016272728712"),
         ("547883752390074072"),
         ("2613731432197218552315486560"))
    TEST(("1939101186844460360481456781574574249267578125"),
         ("600226875930238525467136"),
         ("3230613730581822094172"),
         ("323956730531827184446733"))
    TEST(("733199990780065847944232252876997041821838143"),
         ("8013213820541971659945571336"),
         ("91498867645400738"),
         ("1583515124661363949735792175"))
    TEST(("31912002155274793147956203129900843931859509375"),
         ("3711156506943460547823212032"),
         ("8598937311204313581"),
         ("237615567013935774679302783"))
    TEST(("84817065190325829030474118538864659763146875"),
         ("3989345348920319590101877125"),
         ("21260898160466554"),
         ("3240744994173365678582969625"))
    TEST(("4903174202929413123069116557558039524650036224"),
         ("439519892206178753457503751"),
         ("11155750376434235237"),
         ("103993303446395437906162237"))
    TEST(("7033216625925329185051918302329067850110599168"),
         ("45280486827227264916193129"),
         ("155325552323705124647"),
         ("9543458703673018440648705"))
    TEST(("754099254239656157702242143029442141461010157"),
         ("6750702970137181718814216339"),
         ("111706774476011648"),
         ("4468236274413434030105093485"))
    TEST(("17903941839672196241209232583263392549367477949"),
         ("356994701656891701253476625"),
         ("50151841908510197995"),
         ("256738126978329288513111074"))
    TEST(("28742524270746917988089081078780984685313024"),
         ("7061341439855220048723791808"),
         ("4070405675120028"),
         ("433047040043971055002182400"))
    TEST(("241114331374694647291612992750327612242971875"),
         ("3206517722976157394624000000"),
         ("75195072101738541"),
         ("2893292468706920608658971875"))
    TEST(("36559658086975920957411310021877872011319874893"),
         ("2019943414005990417667711981"),
         ("18099347651759267700"),
         ("1386222149376362340243561193"))
    TEST(("5603776877570350325978679279290675695211252157"),
         ("6270151941359194639085862409"),
         ("893722660946491717"),
         ("849174911039614533291085904"))
    TEST(("749444980285203105724284887490774834262400000"),
         ("7812012161318945645894472197"),
         ("95934947976152423"),
         ("5312562261633417237854716669"))
    TEST(("458612428431662423047527406929285921028369907"),
         ("5356606244996548677516748003"),
         ("85616229279506788"),
         ("1143879966630791183504425543"))
    TEST(("6848941728792754496771176800239159992450309375"),
         ("7486946442513004020353064000"),
         ("914784389254145332"),
         ("159269263651299784953061375"))
    TEST(("1194246198831112018452497653272890352410622976"),
         ("99562226472531255834209411"),
         ("11994972803873553969"),
         ("56164143310713282454420717"))
    TEST(("35387032476818363229992398250619957936938146875"),
         ("7714978075688764345865519939"),
         ("4586796246165500802"),
         ("5058878571946860008886655797"))
    TEST(("3206002606902592847693089932372046107245568"),
         ("6773439551523230104014159208"),
         ("473319734016319"),
         ("2519999008171740428371130216"))
}
#undef TEST

#define TEST(left, right) { \
        big_uint x{ left }; \
        big_uint y{ right }; \
        ASSERT(x < y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x <= y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y > x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y >= x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x != y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y != x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x == x, \
               LIT(x, #left)) \
        ASSERT(y == y, \
               LIT(y, #right)) \
    }
void test_comparisons() {
    TEST(B(0), B(1))
    TEST(B(0), B(2))
    TEST(B(0), B(m))
    TEST(B(0), B(0, 1))
    TEST(B(0), B(1, 1))
    TEST(B(0), B(m, 1))
    TEST(B(0), B(0, 2))
    TEST(B(0), B(m, m))
    TEST(B(0), B(0, 0, 1))
    TEST(B(0), B(1, 0, 1))
    TEST(B(0), B(m, 0, 1))
    TEST(B(0), B(0, 1, 1))
    TEST(B(0), B(0, 0, 2))
    TEST(B(0), B(0, 0, m))
    TEST(B(1), B(2))
    TEST(B(1), B(m))
    TEST(B(1), B(0, 1))
    TEST(B(1), B(1, 1))
    TEST(B(1), B(m, 1))
    TEST(B(1), B(0, 2))
    TEST(B(1), B(m, m))
    TEST(B(1), B(0, 0, 1))
    TEST(B(1), B(1, 0, 1))
    TEST(B(1), B(m, 0, 1))
    TEST(B(1), B(0, 1, 1))
    TEST(B(1), B(0, 0, 2))
    TEST(B(1), B(0, 0, m))
    TEST(B(2), B(m))
    TEST(B(2), B(0, 1))
    TEST(B(2), B(1, 1))
    TEST(B(2), B(m, 1))
    TEST(B(2), B(0, 2))
    TEST(B(2), B(m, m))
    TEST(B(2), B(0, 0, 1))
    TEST(B(2), B(1, 0, 1))
    TEST(B(2), B(m, 0, 1))
    TEST(B(2), B(0, 1, 1))
    TEST(B(2), B(0, 0, 2))
    TEST(B(2), B(0, 0, m))
    TEST(B(m), B(0, 1))
    TEST(B(m), B(1, 1))
    TEST(B(m), B(m, 1))
    TEST(B(m), B(0, 2))
    TEST(B(m), B(m, m))
    TEST(B(m), B(0, 0, 1))
    TEST(B(m), B(1, 0, 1))
    TEST(B(m), B(m, 0, 1))
    TEST(B(m), B(0, 1, 1))
    TEST(B(m), B(0, 0, 2))
    TEST(B(m), B(0, 0, m))
    TEST(B(0, 1), B(1, 1))
    TEST(B(0, 1), B(m, 1))
    TEST(B(0, 1), B(0, 2))
    TEST(B(0, 1), B(m, m))
    TEST(B(0, 1), B(0, 0, 1))
    TEST(B(0, 1), B(1, 0, 1))
    TEST(B(0, 1), B(m, 0, 1))
    TEST(B(0, 1), B(0, 1, 1))
    TEST(B(0, 1), B(0, 0, 2))
    TEST(B(0, 1), B(0, 0, m))
    TEST(B(1, 1), B(m, 1))
    TEST(B(1, 1), B(0, 2))
    TEST(B(1, 1), B(m, m))
    TEST(B(1, 1), B(0, 0, 1))
    TEST(B(1, 1), B(1, 0, 1))
    TEST(B(1, 1), B(m, 0, 1))
    TEST(B(1, 1), B(0, 1, 1))
    TEST(B(1, 1), B(0, 0, 2))
    TEST(B(1, 1), B(0, 0, m))
    TEST(B(0, 2), B(m, m))
    TEST(B(0, 2), B(0, 0, 1))
    TEST(B(0, 2), B(1, 0, 1))
    TEST(B(0, 2), B(m, 0, 1))
    TEST(B(0, 2), B(0, 1, 1))
    TEST(B(0, 2), B(0, 0, 2))
    TEST(B(0, 2), B(0, 0, m))
    TEST(B(m, 1), B(m, m))
    TEST(B(m, 1), B(0, 0, 1))
    TEST(B(m, 1), B(1, 0, 1))
    TEST(B(m, 1), B(m, 0, 1))
    TEST(B(m, 1), B(0, 1, 1))
    TEST(B(m, 1), B(0, 0, 2))
    TEST(B(m, 1), B(0, 0, m))
    TEST(B(m, m), B(0, 0, 1))
    TEST(B(m, m), B(1, 0, 1))
    TEST(B(m, m), B(m, 0, 1))
    TEST(B(m, m), B(0, 1, 1))
    TEST(B(m, m), B(0, 0, 2))
    TEST(B(m, m), B(0, 0, m))
    TEST(B(0, 0, 1), B(1, 0, 1))
    TEST(B(0, 0, 1), B(m, 0, 1))
    TEST(B(0, 0, 1), B(0, 1, 1))
    TEST(B(0, 0, 1), B(0, 0, 2))
    TEST(B(0, 0, 1), B(0, 0, m))
}
#undef TEST

#define TEST_R(left, right) { \
        big_uint x{ left }; \
        digit    y{ right }; \
        ASSERT(x < y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x <= y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y > x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y >= x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x != y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y != x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x == x, \
               LIT(x, #left)) \
        ASSERT(y == y, \
               LIT(y, #right)) \
    }
#define TEST_L(left, right) { \
        digit    x{ left }; \
        big_uint y{ right }; \
        ASSERT(x < y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x <= y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y > x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y >= x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x != y, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(y != x, \
               LIT(x, #left) \
               LIT(y, #right)) \
        ASSERT(x == x, \
               LIT(x, #left)) \
        ASSERT(y == y, \
               LIT(y, #right)) \
    }
void test_comparisons_digit() {
    TEST_R(B(0), 1)
    TEST_R(B(0), 2)
    TEST_R(B(0), 42)
    TEST_R(B(0), m)
    TEST_R(B(1), 2)
    TEST_R(B(1), 42)
    TEST_R(B(1), m)
    TEST_R(B(2), 42)
    TEST_R(B(2), m)
    TEST_R(B(42), m)
    TEST_L(0, B(1))
    TEST_L(0, B(2))
    TEST_L(0, B(42))
    TEST_L(0, B(m))
    TEST_L(0, B(0, 1))
    TEST_L(0, B(1, 1))
    TEST_L(0, B(2, 1))
    TEST_L(0, B(42, 1))
    TEST_L(0, B(m, 1))
    TEST_L(0, B(0, 2))
    TEST_L(0, B(1, 2))
    TEST_L(0, B(42, 2))
    TEST_L(0, B(m, 2))
    TEST_L(0, B(0, 42))
    TEST_L(0, B(1, 42))
    TEST_L(0, B(42, 42))
    TEST_L(0, B(m, 42))
    TEST_L(0, B(0, m))
    TEST_L(0, B(1, m))
    TEST_L(0, B(42, m))
    TEST_L(0, B(m, m))
    TEST_L(1, B(2))
    TEST_L(1, B(42))
    TEST_L(1, B(m))
    TEST_L(1, B(0, 1))
    TEST_L(1, B(1, 1))
    TEST_L(1, B(2, 1))
    TEST_L(1, B(42, 1))
    TEST_L(1, B(m, 1))
    TEST_L(1, B(0, 2))
    TEST_L(1, B(1, 2))
    TEST_L(1, B(42, 2))
    TEST_L(1, B(m, 2))
    TEST_L(1, B(0, 42))
    TEST_L(1, B(1, 42))
    TEST_L(1, B(42, 42))
    TEST_L(1, B(m, 42))
    TEST_L(1, B(0, m))
    TEST_L(1, B(1, m))
    TEST_L(1, B(42, m))
    TEST_L(1, B(m, m))
    TEST_L(2, B(42))
    TEST_L(2, B(m))
    TEST_L(2, B(0, 1))
    TEST_L(2, B(1, 1))
    TEST_L(2, B(2, 1))
    TEST_L(2, B(42, 1))
    TEST_L(2, B(m, 1))
    TEST_L(2, B(0, 2))
    TEST_L(2, B(1, 2))
    TEST_L(2, B(42, 2))
    TEST_L(2, B(m, 2))
    TEST_L(2, B(0, 42))
    TEST_L(2, B(1, 42))
    TEST_L(2, B(42, 42))
    TEST_L(2, B(m, 42))
    TEST_L(2, B(0, m))
    TEST_L(2, B(1, m))
    TEST_L(2, B(42, m))
    TEST_L(2, B(m, m))
    TEST_L(42, B(m))
    TEST_L(42, B(0, 1))
    TEST_L(42, B(1, 1))
    TEST_L(42, B(2, 1))
    TEST_L(42, B(42, 1))
    TEST_L(42, B(m, 1))
    TEST_L(42, B(0, 2))
    TEST_L(42, B(1, 2))
    TEST_L(42, B(42, 2))
    TEST_L(42, B(m, 2))
    TEST_L(42, B(0, 42))
    TEST_L(42, B(1, 42))
    TEST_L(42, B(42, 42))
    TEST_L(42, B(m, 42))
    TEST_L(42, B(0, m))
    TEST_L(42, B(1, m))
    TEST_L(42, B(42, m))
    TEST_L(42, B(m, m))
    TEST_L(m, B(0, 1))
    TEST_L(m, B(1, 1))
    TEST_L(m, B(2, 1))
    TEST_L(m, B(42, 1))
    TEST_L(m, B(m, 1))
    TEST_L(m, B(0, 2))
    TEST_L(m, B(1, 2))
    TEST_L(m, B(42, 2))
    TEST_L(m, B(m, 2))
    TEST_L(m, B(0, 42))
    TEST_L(m, B(1, 42))
    TEST_L(m, B(42, 42))
    TEST_L(m, B(m, 42))
    TEST_L(m, B(0, m))
    TEST_L(m, B(1, m))
    TEST_L(m, B(42, m))
    TEST_L(m, B(m, m))
}
#undef TEST_R
#undef TEST_L

#define TEST(base, exponent, power) { \
        big_uint x{ base }; \
        big_uint y{ power }; \
        big_uint t; \
        ASSERT((t = x.pow(exponent)) == y && t.satisfies_invariant(), \
               LIT(x, #base) \
               LIT(y, #power)) \
    }
void test_pow() {
    TEST(B(0), 0, B(1))
    TEST(B(1), 0, B(1))
    TEST(B(2), 0, B(1))
    TEST(B(42), 0, B(1))
    TEST(B(m), 0, B(1))
    TEST(B(0, 1), 0, B(1))
    TEST(B(1, 1), 0, B(1))
    TEST(B(2, 1), 0, B(1))
    TEST(B(42, 1), 0, B(1))
    TEST(B(m, 1), 0, B(1))
    TEST(B(0, m), 0, B(1))
    TEST(B(1, m), 0, B(1))
    TEST(B(2, m), 0, B(1))
    TEST(B(42, m), 0, B(1))
    TEST(B(m, m), 0, B(1))
    TEST(B(432, 432, 32, 42, 3214, 32), 0, B(1))
    TEST(B(0), 1, B(0))
    TEST(B(1), 1, B(1))
    TEST(B(2), 1, B(2))
    TEST(B(42), 1, B(42))
    TEST(B(m), 1, B(m))
    TEST(B(0, 1), 1, B(0, 1))
    TEST(B(1, 1), 1, B(1, 1))
    TEST(B(2, 1), 1, B(2, 1))
    TEST(B(42, 1), 1, B(42, 1))
    TEST(B(m, 1), 1, B(m, 1))
    TEST(B(0, m), 1, B(0, m))
    TEST(B(1, m), 1, B(1, m))
    TEST(B(2, m), 1, B(2, m))
    TEST(B(42, m), 1, B(42, m))
    TEST(B(m, m), 1, B(m, m))
    TEST(B(432, 432, 32, 42, 3214, 32), 1, B(432, 432, 32, 42, 3214, 32))
    TEST(B(0), 2, B(0))
    TEST(B(1), 2, B(1))
    TEST(B(2), 2, B(4))
    TEST(B(42), 2, B(1764))
    TEST(B(m), 2, B(1, m - 1))
    TEST(B(0, 1), 2, "18446744073709551616")
    TEST(B(1, 1), 2, "18446744082299486209")
    TEST(B(2, 1), 2, "18446744090889420804")
    TEST(B(42, 1), 2, "18446744434486806244")
    TEST(B(m, 1), 2, "73786976277658337281")
    TEST(B(0, m), 2, "340282366762482138453292676318389862400")
    TEST(B(1, m), 2, "340282366762482138490186164457219031041")
    TEST(B(2, m), 2, "340282366762482138527079652596048199684")
    TEST(B(42, m), 2, "340282366762482140002819178149214947044")
    TEST(B(m, m), 2, "340282366920938463426481119284349108225")
    TEST("3143271849327891473289789789374282", 2, 
         "9880157918777182876145376146391788801255400268620362724753083015524")
    TEST("4738291477084372891789473829478932", 2, 
         "22451406121810408237118933033938067953672353960296953800970631860624")
    TEST("7832748932748732984789328978978924", 2, 
         "61351955843476415598484178692699172363218502316314342645877636197776")
    TEST("7832748932748732984789328978978924", 3,
         "48055446665503728646756430172310507262823159221860175454456200861257"
         "9264593367350696903455090199673024")
    TEST("7832748932748732984789328978978924", 4,
         "37640624858198798981714624695996812167887317208197410485913958181064"
         "81168875242192851932685244873236552032333091715547210686714187346176")
    TEST("7832748932748732984789328978978924", 5,
         "29482956418605207156712251373059597153824942108112400032339679726656"
         "45136058269078956482802131301333301516384872346107400036428040047646"
         "0966927514522819522633421795994624")
    TEST("7832748932748732984789328978978924", 6,
         "23093259542210734324609262533320917475849421542615409832514098315312"
         "22100837534767864572262286162529648450618120466513746086210888858904"
         "86292357949822502387270896203271156294892685348727776853636513304576")
    TEST("7832748932748732984789328978978924", 7,
         "18088370403294062334557303846862144133200491268479281763204663082973"
         "82434279258115610102744787272625650576199856503998552392704568689649"
         "18352036357484972031461188848177029741027438166989380281999867222073"
         "9249387133844193637646953496756224")
    TEST("7832748932748732984789328978978924", 42,
         "35026492989601039592147227325755225666528894742602967040639035962137"
         "04320384451845352938408298077940116694022130835438219878208939892015"
         "47516750405226804098298728250457240396002310404463577985746003637297"
         "08121509419251704353636136410973493767938960743752627290249655457837"
         "16475439568984837375088036151588314013919875196944263257877299015320"
         "27142252288429330743356320851596266317411136530132133519779544466867"
         "92545367581436686949053054724169598323601591694461445300084682047506"
         "89265518891382466461360232877568818761013109142916446760126277844020"
         "12218032967123109940349052264732907265178285283773658514571970124605"
         "87841263276565097360250677649052318713061473422539572410648413141983"
         "14938156307269065380990972076896425100193525654107804131584069283074"
         "69997761832220490015998826059901455855960693080622017171837868292500"
         "96374805845792703627798654075854840742895269099772365701644459433804"
         "90479464111450083801577399833769325571585288161473442929692427208963"
         "27541034602287276214335937952642263227216215586316391514853635418902"
         "24605220380808027650729154132285461603219012971014458282428246941971"
         "26193805471297811725476268337783996534767464080944202435423974959864"
         "06377717455447824605192039700353185558278132719172461930806230657802"
         "04459557443501028483950812303716636297357168637985296609701443095963"
         "20753316013331050285074189624077075088472454889809524238044841589663"
         "3994015881285443648764403952305187580493448759701619748714315776")
}
#undef TEST

int main() {
    test_constructors();
    test_increment_and_decrement();
    test_add_digit();
    test_subtract_digit();
    test_reverse_subtract_digit();
    test_multiply_digit();
    test_divide_digit();
    test_reverse_divide_digit();
    test_add_and_subtract();
    test_multiply();
    test_divide();
    test_comparisons();
    test_comparisons_digit();
    test_pow();
    cout << "OK!" << endl;
}
