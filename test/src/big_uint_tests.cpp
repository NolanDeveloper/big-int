#include "big_uint.hpp"
#include "assert.hpp"

#include <tuple>
#include <utility>
#include <iterator>
#include <limits>
#include <vector>
#include <sstream>
#include <cassert>

using namespace big;
using namespace std;

static const digit      m  = numeric_limits<digit>::max();
static const long_digit mm = numeric_limits<long_digit>::max();

void test_constructor(big_uint x, deque<digit> y) {
    assert(x.digits() == y);
    assert(x.satisfies_invariant());
}

void test_constructors() {
    test_constructor({}, { 0 });
    test_constructor({ 1 }, { 1 });
    test_constructor({ 432 }, { 432 });
    test_constructor({ "0" }, { 0 });
    test_constructor({ "123" }, { 123 });
    test_constructor({ "100" }, { 100 });
    test_constructor({ "999" }, { 999 });
    test_constructor({ "256" }, { 256 });
    test_constructor({ "257" }, { 257 });
    test_constructor({ "255" }, { 255 });
    test_constructor({ to_string(m) }, { m });
    test_constructor({ to_string((long_digit) m + 1) }, { 0, 1 });
    test_constructor({ to_string(mm) }, { m, m });
    test_constructor({ "340282367000166625996085689103316680705" }, { 1, 1, 1, 1, 1});
    test_constructor({ "339942088881935131302157980349511352979684" }, 
            { 4324, 5453, 43262, 54626, 999 });
    test_constructor({ "00000000001024" }, { 1024 });
    test_constructor({ "18446744090889420804" }, { 4, 4, 1 });
}

void test_predecrement(big_uint prev, big_uint next) {
    assert(prev == --next);
    assert(next.satisfies_invariant());
}

void test_postdecrement(big_uint prev, big_uint next) {
    assert((next--, prev == next));
    assert(next.satisfies_invariant());
}

void test_preincrement(big_uint prev, big_uint next) {
    assert(++prev == next);
    assert(prev.satisfies_invariant());
}

void test_postincrement(big_uint prev, big_uint next) {
    assert((prev++, prev == next));
    assert(prev.satisfies_invariant());
}

void test_increment_and_decrement(big_uint prev, big_uint next) {
    test_predecrement(prev, next);
    test_postdecrement(prev, next);
    test_preincrement(prev, next);
    test_postincrement(prev, next);
}

void test_increment_and_decrement() {
    test_increment_and_decrement({ 0 }, { 1 });
    test_increment_and_decrement({ 41 }, { 42 });
    test_increment_and_decrement({ 42 }, { 43 });
    test_increment_and_decrement({ 127 }, { 128 });
    test_increment_and_decrement({ 128 }, { 129 });
    test_increment_and_decrement({ m }, { 0, 1 });
    test_increment_and_decrement({ m, m }, { 0, 0, 1 });
    test_increment_and_decrement({ 0, 0, 1 }, { 1, 0, 1 });
    test_increment_and_decrement({ m, 0, 1 }, { 0, 1, 1 });
    test_increment_and_decrement({ 42, 42 }, { 43, 42 });
}

void test_add_digit(const big_uint & augend, digit audend, const big_uint & sum) {
    big_uint t = augend + audend;
    assert(t == sum);
    assert(t.satisfies_invariant());
    t = audend + augend;
    assert(t == sum);
    assert(t.satisfies_invariant());
    t = augend;
    t += audend;
    assert(t == sum);
    assert(t.satisfies_invariant());
}

void test_add_digit() {
    test_add_digit({ 0 }, 0, { 0 });
    test_add_digit({ 0 }, 1, { 1 });
    test_add_digit({ 1 }, 0, { 1 });
    test_add_digit({ 1 }, 1, { 2 });
    test_add_digit({ m }, 0, { m });
    test_add_digit({ 0 }, m, { m });
    test_add_digit({ m }, 1, { 0, 1 });
    test_add_digit({ 1 }, m, { 0, 1 });
    test_add_digit({ m - 41 }, 42, { 0, 1 });
    test_add_digit({ 42 }, m - 41, { 0, 1 });
    test_add_digit({ m }, 42, { 41, 1 });
    test_add_digit({ 42 }, m, { 41, 1 });
    test_add_digit({ m }, m, { m - 1, 1 });
    test_add_digit({ 0, 1 }, 1, { 1, 1 });
    test_add_digit({ 1, 1 }, 1, { 2, 1 });
    test_add_digit({ m, 42 }, 1, { 0, 43 });
    test_add_digit({ m, 42 }, 42, { 41, 43 });
    test_add_digit({ m - 41, 42 }, 42, { 0, 43 });
    test_add_digit({ m, m }, 1, { 0, 0, 1 });
    test_add_digit({ m, m, m }, 1, { 0, 0, 0, 1 });
    test_add_digit({ m, m, m }, 0, { m, m, m });
    test_add_digit({ m, m, m }, 100, { 99, 0, 0, 1 });
    test_add_digit({ m, m, 5 }, 100, { 99, 0, 6 });
    test_add_digit({ "3243267419327147878943294639276" }, 0, 
            { "3243267419327147878943294639276" });
}

void test_subtract_digit(const big_uint & minuend, digit subtrahend, 
        const big_uint & difference) {
    big_uint t = minuend - subtrahend;
    assert(t == difference);
    assert(t.satisfies_invariant());
    t = minuend;
    t -= subtrahend;
    assert(t == difference);
    assert(t.satisfies_invariant());
}

void test_subtract_digit() {
    test_subtract_digit({ 0 }, 0, { 0 });
    test_subtract_digit({ 1 }, 0, { 1 });
    test_subtract_digit({ 1 }, 1, { 0 });
    test_subtract_digit({ 50 }, 5, { 45 });
    test_subtract_digit({ m }, m, { 0 });
    test_subtract_digit({ m }, m - 1, { 1 });
    test_subtract_digit({ m }, 1, { m - 1 });
    test_subtract_digit({ 0, 1 }, 42, { m - 41 });
    test_subtract_digit({ 0, 1 }, 1, { m });
    test_subtract_digit({ 0, 42 }, 1, { m, 41 });
    test_subtract_digit({ 0, 42 }, 42, { m - 41, 41 });
    test_subtract_digit({ 0, 0, 0, 0, 0, 1 }, 1, { m, m, m, m, m });
    test_subtract_digit({ 0, 0, 0, 0, 0, 1 }, m, { 1, m, m, m, m });
}

void test_reverse_subtract_digit(digit minuend, const big_uint & subtrahend, 
        const big_uint & difference) {
    big_uint t = minuend - subtrahend;
    assert(t == difference);
    assert(t.satisfies_invariant());
}

void test_reverse_subtract_digit() {
    test_reverse_subtract_digit(0, { 0 }, { 0 });
    test_reverse_subtract_digit(1, { 0 }, { 1 });
    test_reverse_subtract_digit(42, { 0 }, { 42 });
    test_reverse_subtract_digit(m, { 0 }, { m });
    test_reverse_subtract_digit(1, { 1 }, { 0 });
    test_reverse_subtract_digit(42, { 1 }, { 41 });
    test_reverse_subtract_digit(m, { 1 }, { m - 1 });
    test_reverse_subtract_digit(2, { 2 }, { 0 });
    test_reverse_subtract_digit(42, { 2 }, { 40 });
    test_reverse_subtract_digit(m, { 2 }, { m - 2 });
    test_reverse_subtract_digit(m, { m }, { 0 });
}

void test_multiply_digit(const big_uint & multiplier, digit multiplicand, 
        const big_uint & product) {
    big_uint t = multiplier * multiplicand;
    assert(t == product);
    assert(t.satisfies_invariant());
    t = multiplicand * multiplier;
    assert(t == product);
    assert(t.satisfies_invariant());
    t = multiplier;
    t *= multiplicand;
    assert(t == product);
    assert(t.satisfies_invariant());
}

void test_multiply_digit() {
    auto l = [] (long_digit d) -> digit { return d; };
    auto h = [] (long_digit d) -> digit { return d >> (8 * sizeof(digit)); };
    test_multiply_digit({ 0 }, 0, { 0 });
    test_multiply_digit({ 0 }, 1, { 0 });
    test_multiply_digit({ 0 }, 42, { 0 });
    test_multiply_digit({ 1 }, 0, { 0 });
    test_multiply_digit({ 42 }, 0, { 0 });
    test_multiply_digit({ 0, 1 }, 0, { 0 });
    test_multiply_digit({ 0, 42 }, 0, { 0 });
    test_multiply_digit({ 1 }, 1, { 1 });
    test_multiply_digit({ 42 }, 1, { 42 });
    test_multiply_digit({ 1 }, 42, { 42 });
    test_multiply_digit({ 0, 1 }, 42, { 0, 42 });
    test_multiply_digit({ 1, 1 }, 42, { 42, 42 });
    test_multiply_digit({ 2 }, 2, { 4 });
    test_multiply_digit({ 3 }, 2, { 6 });
    test_multiply_digit({ 2 }, 3, { 6 });
    test_multiply_digit({ 2 }, 3, { 6 });
    test_multiply_digit({ 2 }, m, { 2 * m, 1 });
    test_multiply_digit({ m }, 2, { 2 * m, 1 });
    test_multiply_digit({ m }, m, { l((long_digit) m * m), h((long_digit) m * m) });
    test_multiply_digit({ 1, m }, m, { m, l((long_digit) m * m), h((long_digit) m * m) });
    test_multiply_digit({ 0, m }, m, { 0, l((long_digit) m * m), h((long_digit) m * m) });
}

void test_divide_digit(const big_uint & dividend, digit divisor, const big_uint & quotient, 
        digit reminder) {
    big_uint t = dividend / divisor;
    assert(t == quotient);
    assert(t.satisfies_invariant());
    t = dividend;
    t /= divisor;
    assert(t == quotient);
    assert(t.satisfies_invariant());
    big_uint r = dividend % divisor;
    assert(r == reminder);
}

void test_divide_digit() {
    test_divide_digit({ 0 }, 1, { 0 }, 0);
    test_divide_digit({ 0 }, 2, { 0 }, 0);
    test_divide_digit({ 0 }, 100, { 0 }, 0);
    test_divide_digit({ 0 }, 1, { 0 }, 0);
    test_divide_digit({ 1 }, 1, { 1 }, 0);
    test_divide_digit({ 1 }, 2, { 0 }, 1);
    test_divide_digit({ 2 }, 2, { 1 }, 0);
    test_divide_digit({ 4 }, 2, { 2 }, 0);
    test_divide_digit({ m }, 2, { m / 2 }, 1);
    test_divide_digit({ 0, 1 }, 2, { m / 2 + 1 }, 0);
    test_divide_digit({ 0, m }, 2, { 1u << (8 * sizeof(digit) - 1), m >> 1 }, 0);
    test_divide_digit({ 0, m }, 4, { 3u << (8 * sizeof(digit) - 2), m >> 2 }, 0);
    test_divide_digit({ "311543243254325435435441361748615345432543254325432543254325" }, 
            54354, { "5731744549698742234894237070843274559968783425790788962" }, 13777);
    test_divide_digit({ "43621874963271894632871946721389468932164" }, 2, 
         { "21810937481635947316435973360694734466082" }, 0);
    test_divide_digit({ "21810937481635947316435973360694734466082" }, 2,
         { "10905468740817973658217986680347367233041" }, 0);
    test_divide_digit({ "10905468740817973658217986680347367233041" }, 2,
         { "5452734370408986829108993340173683616520" }, 1);
    test_divide_digit({ "5452734370408986829108993340173683616520" }, 2,
         { "2726367185204493414554496670086841808260" }, 0);
    test_divide_digit({ "2726367185204493414554496670086841808260" }, 2,
         { "1363183592602246707277248335043420904130" }, 0);
    test_divide_digit({ "1363183592602246707277248335043420904130" }, 2,
         { "681591796301123353638624167521710452065" }, 0);
    test_divide_digit({ "681591796301123353638624167521710452065" }, 2,
         { "340795898150561676819312083760855226032" }, 1);
    test_divide_digit({ "681591796301123353638624167521710452065" }, 43432432,
         { "15693152902446801819401321287320" }, 32089825);
}

void test_reverse_divide_digit(digit dividend, const big_uint & divisor, 
        const big_uint & quotient, digit reminder) {
    big_uint t = dividend / divisor;
    assert(t == quotient);
    assert(t.satisfies_invariant());
    t = dividend;
    t /= divisor;
    assert(t == quotient);
    assert(t.satisfies_invariant());
    t = dividend % divisor;
    assert(t == reminder);
    t = dividend;
    t %= divisor;
    assert(t == reminder);
    assert(t.satisfies_invariant());
}

void test_reverse_divide_digit() {
    test_reverse_divide_digit(0, { 1 }, { 0 }, 0);
    test_reverse_divide_digit(1, { 1 }, { 1 }, 0);
    test_reverse_divide_digit(2, { 1 }, { 2 }, 0);
    test_reverse_divide_digit(42, { 1 }, { 42 }, 0);
    test_reverse_divide_digit(m, { 1 }, { m }, 0);
    test_reverse_divide_digit(1, { 2 }, { 0 }, 1);
    test_reverse_divide_digit(2, { 2 }, { 1 }, 0);
    test_reverse_divide_digit(42, { 2 }, { 21 }, 0);
    test_reverse_divide_digit(43, { 2 }, { 21 }, 1);
    test_reverse_divide_digit(m, { 2 }, { m >> 1 }, 1);
    test_reverse_divide_digit(m - 1, { 2 }, { m >> 1 }, 0);
    test_reverse_divide_digit(0, { m }, { 0 }, 0);
    test_reverse_divide_digit(1, { m }, { 0 }, 1);
    test_reverse_divide_digit(2, { m }, { 0 }, 2);
    test_reverse_divide_digit(m - 1, { m }, { 0 }, m - 1);
    test_reverse_divide_digit(m, { m }, { 1 }, 0);
}

void test_add(const big_uint & augend, const big_uint & audend, 
        const big_uint & sum) {
    big_uint t = augend + audend;
    assert(t == sum);
    assert(t.satisfies_invariant());
    t = augend;
    t += audend;
    assert(t == sum);
    assert(t.satisfies_invariant());
    t = audend + augend;
    assert(t == sum);
    assert(t.satisfies_invariant());
    t = audend;
    t += augend;
    assert(t == sum);
    assert(t.satisfies_invariant());
}

void test_subtract(const big_uint & augend, const big_uint & audend, 
        const big_uint & sum) {
    big_uint t = sum - augend;
    assert(t == audend);
    assert(t.satisfies_invariant());
    t = sum;
    t -= augend;
    assert(t == audend);
    assert(t.satisfies_invariant());
    t = sum - audend;
    assert(t == augend);
    assert(t.satisfies_invariant());
    t = sum;
    t -= audend;
    assert(t == augend);
    assert(t.satisfies_invariant());
}

void test_add_and_subtract(const big_uint & augend, const big_uint & audend, 
        const big_uint & sum) {
    test_add(augend, audend, sum);
    test_subtract(augend, audend, sum);
}

void test_add_and_subtract() {
    test_add_and_subtract({ 0 }, { 0 }, { 0 });
    test_add_and_subtract({ 0 }, { 1 }, { 1 });
    test_add_and_subtract({ 0 }, { 42 }, { 42 });
    test_add_and_subtract({ 0 }, { 0, 1 }, { 0, 1 }); 
    test_add_and_subtract({ 0 }, { 0, 42 }, { 0, 42 });
    test_add_and_subtract({ 0 }, { 43, 42 }, { 43, 42 });
    test_add_and_subtract({ 0 }, { 0, 0, 1 }, { 0, 0, 1 });
    test_add_and_subtract({ 0 }, { 0, 0, 42 }, { 0, 0, 42 });
    test_add_and_subtract({ 0 }, { 0, 43, 42 }, { 0, 43, 42 });
    test_add_and_subtract({ 0 }, { 44, 43, 42 }, { 44, 43, 42 });
    test_add_and_subtract({ 0 }, { 44, 0, 42 }, { 44, 0, 42 });
    test_add_and_subtract({ 0 }, { m }, { m });
    test_add_and_subtract({ 0 }, { m, m }, { m, m });
    test_add_and_subtract({ 0 }, { m, m, 1 }, { m, m, 1 });
    test_add_and_subtract({ 0 }, { m, 0, m }, { m, 0, m });
    test_add_and_subtract({ 1 }, { 1 }, { 2 });
    test_add_and_subtract({ 1 }, { 41 }, { 42 });
    test_add_and_subtract({ 1 }, { m }, { 0, 1 });
    test_add_and_subtract({ 1 }, { m, m }, { 0, 0, 1 });
    test_add_and_subtract({ 1 }, { m, m, m }, { 0, 0, 0, 1 });
    test_add_and_subtract({ 1 }, { m, m, m, m }, { 0, 0, 0, 0, 1 });
    test_add_and_subtract({ 42 }, { 128 }, { 170 });
    test_add_and_subtract({ 42 }, { m }, { 41, 1 });
    test_add_and_subtract({ 42 }, { m, m }, { 41, 0, 1 });
    test_add_and_subtract({ 42 }, { m, m, m }, { 41, 0, 0, 1 });
    test_add_and_subtract({ 3555 }, { 32134 - 3555 }, { 32134 });
    test_add_and_subtract({ 0, 1 }, { 0, 1 }, { 0, 2 });
    test_add_and_subtract({ 0, 42 }, { 0, 43 }, { 0, 85 });
    test_add_and_subtract({ 1, 1 }, { 1, 1 }, { 2, 2 });
    test_add_and_subtract({ 42, 1 }, { 43, 1 }, { 85, 2 });
    test_add_and_subtract({ m, m, m }, { m, m, m }, { m - 1, m, m, 1 });
    test_add_and_subtract({ "5412218217130331892" }, { "23213214343532432" }, 
            { "5435431431473864324" });
    test_add_and_subtract({ "7427255953219916513991965128" }, 
            { "76353637546496645509677056" }, { "7503609590766413159501642184" });
    test_add_and_subtract({ "4117099328278639779421887554" }, 
            { "373069782234956328652655603" }, { "4490169110513596108074543157" });
}

void test_multiply_one_way(const big_uint & multiplier, const big_uint & multiplicand, 
        const big_uint & product) {
    big_uint t = multiplier * multiplicand;
    assert(t == product);
    assert(t.satisfies_invariant());
    t = multiplier;
    t *= multiplicand;
    assert(t == product);
    assert(t.satisfies_invariant());
}

void test_multiply(const big_uint & multiplier, const big_uint & multiplicand, 
        const big_uint & product) {
    test_multiply_one_way(multiplier, multiplicand, product);
    test_multiply_one_way(multiplicand, multiplier, product);
}

void test_multiply() {
    test_multiply({ 0 }, { 0 }, { 0 });
    test_multiply({ 0 }, { 1 }, { 0 });
    test_multiply({ 0 }, { 42 }, { 0 });
    test_multiply({ 0 }, { 0, 42 }, { 0 });
    test_multiply({ 0 }, { 43, 42 }, { 0 });
    test_multiply({ 0 }, { 43, 42, 41 }, { 0 });
    test_multiply({ 0 }, { 43, 0, 41 }, { 0 });
    test_multiply({ 1 }, { 1 }, { 1 });
    test_multiply({ 1 }, { 42 }, { 42 });
    test_multiply({ 1 }, { 0, 42 }, { 0, 42 });
    test_multiply({ 1 }, { 43, 42 }, { 43, 42 });
    test_multiply({ 1 }, { 43, 42, 41 }, { 43, 42, 41 });
    test_multiply({ 1 }, { 43, 0, 41 }, { 43, 0, 41 });
    test_multiply({ 1 }, { 43, 0, 41 }, { 43, 0, 41 });
    test_multiply({ 2 }, { 1 }, { 2 });
    test_multiply({ 2 }, { 2 }, { 4 });
    test_multiply({ 2 }, { m }, { 2 * m, 1 });
    test_multiply({ 4 }, { m }, { 4 * m, 3 });
    test_multiply({ 8 }, { m }, { 8 * m, 7 });
    test_multiply({ m }, { m }, { 1, m - 1 });
    test_multiply({ m }, { m, m }, { 1, m, m - 1 });
    test_multiply({ 0, 1 }, { 0, 1 }, { 0, 0, 1 });
    test_multiply({ m, m }, { m, m }, { 1, 0, m - 1, m });
    test_multiply({ m, m, m, m, m }, { m, m, m }, { 1, 0, 0, m, m, m - 1, m, m });
    test_multiply({ "32153214732946372463729643826473821964783921648" }, 
            { "4324324234136748316481" }, 
            { "139040925675082734651478887262280244116520657008305538586301011080688" });
}

void test_divide(const big_uint & dividend, const big_uint & divisor, 
        const big_uint & quotient) {
    big_uint t = dividend / divisor;
    assert(t == quotient);
    assert(t.satisfies_invariant());
    t = dividend;
    t /= divisor;
    assert(t == quotient);
    assert(t.satisfies_invariant());
}

void test_modulo(const big_uint & dividend, const big_uint & divisor, 
        const big_uint & reminder) {
    big_uint t = dividend % divisor;
    assert(t == reminder);
    assert(t.satisfies_invariant());
    t = dividend;
    t %= divisor;
    assert(t == reminder);
    assert(t.satisfies_invariant());
}

void test_divide(const big_uint & dividend, const big_uint & divisor, 
        const big_uint & quotient, const big_uint & reminder) {
    test_divide(dividend, divisor, quotient);
    test_modulo(dividend, divisor, reminder);
}

void test_divide() {
    test_divide({ 0 }, { 1 }, { 0 }, { 0 });
    test_divide({ 1 }, { 2 }, { 0 }, { 1 });
    test_divide({ 100 }, { 101 }, { 0 }, { 100 });
    test_divide({ 3242 }, { 3242342 }, { 0 }, { 3242 });
    test_divide({ m - 1 }, { m }, { 0 }, { m - 1 });
    test_divide({ 1 }, { m }, { 0 }, { 1 });
    test_divide({ 1 }, { 0, 1 }, { 0 }, { 1 });
    test_divide({ 1 }, { 1, 1 }, { 0 }, { 1 });
    test_divide({ 0, 1 }, { 1, 1 }, { 0 }, { 0, 1 });
    test_divide({ 0, 1 }, { m, 1 }, { 0 }, { 0, 1 });
    test_divide({ m - 1, m }, { m, m }, { 0 }, { m - 1, m });
    test_divide({ m - 1, m }, { m, m, m, m, m }, { 0 }, { m - 1, m });
    test_divide({ 1 }, { 1 }, { 1 }, { 0 });
    test_divide({ m }, { m }, { 1 }, { 0 });
    test_divide({ 100 }, { 100 }, { 1 }, { 0 });
    test_divide({ 0, 1 }, { 0, 1 }, { 1 }, { 0 });
    test_divide({ m, m, m }, { m, m, m }, { 1 }, { 0 });
    test_divide({ 2 }, { 1 }, { 2 }, { 0 });
    test_divide({ 3 }, { 1 }, { 3 }, { 0 });
    test_divide({ 42 }, { 1 }, { 42 }, { 0 });
    test_divide({ m }, { 1 }, { m }, { 0 });
    test_divide({ m, m }, { 1 }, { m, m }, { 0 });
    test_divide({ m, m, m, m }, { 1 }, { m, m, m, m }, { 0 });
    test_divide({ 4 }, { 2 }, { 2 }, { 0 });
    test_divide({ 12 }, { 4 }, { 3 }, { 0 });
    test_divide({ 40 }, { 10 }, { 4 }, { 0 });
    test_divide({ m }, { 2 }, { m / 2 }, { 1 });
    test_divide({ m }, { 3 }, { m / 3 }, { m % 3 });
    test_divide({ m }, { 42 }, { m / 42 }, { m % 42 });
    test_divide({ m, m }, { 2 }, { m, m / 2 }, { 1 });
    test_divide({ m, m }, { 4 }, { m, m / 4 }, { 3 });
    test_divide({ m, m }, { 8 }, { m, m / 8 }, { 7 });
    test_divide({ m, m }, { 16 }, { m, m / 16 }, { 15 });
    test_divide({ m, m }, { m }, { 1, 1 }, { 0 });
    test_divide({ m, m, m, m }, { m, m }, { 1, 0, 1 }, { 0 });
    test_divide({ 64 }, { 2 }, { 32 }, { 0 });
    test_divide({ 0, 64 }, { 2 }, { 0, 32 }, { 0 });
    test_divide({ 0, 64 }, { m }, { 64 }, { 64 });
    test_divide({ m, m, m, m, 64 }, { m, m }, { 65, 0, 65 }, { 64 });
    test_divide({ "193337807559688298930754147171641093868975707" }, 
            { "4490169110513596108074543157" }, 
            { "43058023606949152" }, 
            { "1933748539936698160940422843" });
    test_divide({ "75528846334302370804827124168427154062500000" }, 
            { "1683636112060604732504683448" }, 
            { "44860552582150607" }, 
            { "447653616291209758466447064" });
    test_divide({ "1132525508412777091109288731362817259853788043" }, 
            { "2463055200723804344752501952" }, 
            { "459805167208582297" }, 
            { "2090271824848574522208644299" });
    test_divide({ "295006136719563272045078555355314325000044451" }, 
            { "1340168791122082979216216000" }, 
            { "220126105512846270" }, 
            { "646047603915110425885724451" });
    test_divide({ "35756746887336380751538542246333586104625357824" }, 
            { "7616464909621040695721928983" }, 
            { "4694664429185359106" }, 
            { "843662872430705616740988626" });
    test_divide({ "4741878768312026933022886262165697667042658701" }, 
            { "3654992685691111205162875483" }, 
            { "1297370248338923780" }, 
            { "478364294338710608674972961" });
    test_divide({ "2600188226511891395478053392718237397137093" }, 
            { "2214567202119851126722000553" }, 
            { "1174129294438620" }, 
            { "1822481037247953933932580233" });
    test_divide({ "52560992936204269321896054508110584832" }, 
            { "141906137908840773415628409" }, 
            { "370392667369" }, 
            { "115567489776503227266898911" });
    test_divide({ "2222898338643105118962354568339698257133568" }, 
            { "5150437746469652216012707000" }, 
            { "431594060168338" }, 
            { "2862900225027880131186167568" });
    test_divide({ "4312255903554341144066849577228089065789143" }, 
            { "638325266344800352689264291" }, 
            { "6755577651261285" }, 
            { "604165436825163714204515208" });
    test_divide({ "1682143229012189349805232082310945588290543" }, 
            { "12757973275316991201026624" }, 
            { "131850349010109048" }, 
            { "10097045870352413796996591" });
    test_divide({ "45426857196788382413931916718693308253171053568" }, 
            { "102965937463351835666579456" }, 
            { "441183349716569543355" }, 
            { "19233014713321432426738688" });
    test_divide({ "1621623272887369323359690431656860474019300000" }, 
            { "5851264858867636842717814627" }, 
            { "277140637452052233" }, 
            { "4758661388498505184003887909" });
    test_divide({ "3941626601019122942408291156204078388197406343" }, 
            { "256923537735125398888494379" }, 
            { "15341632906684991388" }, 
            { "61136043202959592295998291" });
    test_divide({ "4810921854995887810714597765554820688330700000" }, 
            { "1427241079981484551656197632" }, 
            { "3370784321215235336" }, 
            { "285354503877361762524775648" });
    test_divide({ "1337981899422943908907000637111098478076264224" }, 
            { "9116472605804618828260289993" }, 
            { "146765306854652011" }, 
            { "4586689973978595146115638301" });
    test_divide({ "97814509989165608223697727684258329184248832" }, 
            { "4545242415007334558919545507" }, 
            { "21520196517176909" }, 
            { "4228987060631810032189150969" });
    test_divide({ "1852044222687172754512244972653265714434300000" }, 
            { "4519766203612463069074069624" }, 
            { "409765492119240603" }, 
            { "143507974061027992804556728" });
    test_divide({ "469018580489436804052981776271238180425985632" }, 
            { "7017730422423651698263488000" }, 
            { "66833370941521004" }, 
            { "230149064981333902124033632" });
    test_divide({ "83367202253609923934141183321621299392365625" }, 
            { "433623688284936030965856872" }, 
            { "192257029553304682" }, 
            { "211429884667762421972890921" });
    test_divide({ "15702386492996888950308262150822441066202475999" }, 
            { "7687297046420938881733078125" }, 
            { "2042640787545425367" }, 
            { "6863950899586122958834679124" });
    test_divide({ "75428358360098820558835293615837802392576" }, 
            { "8472999854175697904553936811" }, 
            { "8902202249292" }, 
            { "5134674351346207019964904764" });
    test_divide({ "1838903138206502729914814698293531335217049824" }, 
            { "6311473551050869632524183" }, 
            { "291358764848237802052" }, 
            { "4713467368213414560026308" });
    test_divide({ "11983196336932085645125131770676927699295232" }, 
            { "2889189632750754142906320141" }, 
            { "4147597721207058" }, 
            { "973997464386047032202540054" });
    test_divide({ "25536819334700733676710684837746486802851791557" }, 
            { "420596955138084987703975624" }, 
            { "60715654316415137377" }, 
            { "316779037473587925832493309" });
    test_divide({ "49699753337881641431263771469436134910543" }, 
            { "79502401625653519277211" }, 
            { "625135245245782894" }, 
            { "21288823458503427081909" });
    test_divide({ "917407177119868685051081742909017192847334757" }, 
            { "2487664168524508426709768" }, 
            { "368782566685439801374" }, 
            { "1774643949396578581713525" });
    test_divide({ "35082204086211562347881877456499751577311776" }, 
            { "9056414127365363740365782776" }, 
            { "3873741151059471" }, 
            { "3610175119377778242933840280" });
    test_divide({ "27301271478031964520914855746528826777302678125" }, 
            { "6101764819983846018047959552" }, 
            { "4474323787212802257" }, 
            { "1241877880020294826392369261" });
    test_divide({ "61586499853823005267722354972735539669585457" }, 
            { "1557372533965684033851162216" }, 
            { "39545130346558452" }, 
            { "51688864597353083691735825" });
    test_divide({ "339628286150170618468395797227784036124019007" }, 
            { "4351677652590841046371717000" }, 
            { "78045368536883120" }, 
            { "217563254770815933406979007" });
    test_divide({ "1615356973582482194670478770106867527556641824" }, 
            { "2948357140608186016272728712" }, 
            { "547883752390074072" }, 
            { "2613731432197218552315486560" });
    test_divide({ "1939101186844460360481456781574574249267578125" }, 
            { "600226875930238525467136" }, 
            { "3230613730581822094172" }, 
            { "323956730531827184446733" });
    test_divide({ "733199990780065847944232252876997041821838143" }, 
            { "8013213820541971659945571336" }, 
            { "91498867645400738" }, 
            { "1583515124661363949735792175" });
    test_divide({ "31912002155274793147956203129900843931859509375" }, 
            { "3711156506943460547823212032" }, 
            { "8598937311204313581" }, 
            { "237615567013935774679302783" });
    test_divide({ "84817065190325829030474118538864659763146875" }, 
            { "3989345348920319590101877125" }, 
            { "21260898160466554" }, 
            { "3240744994173365678582969625" });
    test_divide({ "4903174202929413123069116557558039524650036224" }, 
            { "439519892206178753457503751" }, 
            { "11155750376434235237" }, 
            { "103993303446395437906162237" });
    test_divide({ "7033216625925329185051918302329067850110599168" }, 
            { "45280486827227264916193129" }, 
            { "155325552323705124647" }, 
            { "9543458703673018440648705" });
    test_divide({ "754099254239656157702242143029442141461010157" }, 
            { "6750702970137181718814216339" }, 
            { "111706774476011648" }, 
            { "4468236274413434030105093485" });
    test_divide({ "17903941839672196241209232583263392549367477949" }, 
            { "356994701656891701253476625" }, 
            { "50151841908510197995" }, 
            { "256738126978329288513111074" });
    test_divide({ "28742524270746917988089081078780984685313024" }, 
            { "7061341439855220048723791808" }, 
            { "4070405675120028" }, 
            { "433047040043971055002182400" });
    test_divide({ "241114331374694647291612992750327612242971875" }, 
            { "3206517722976157394624000000" }, 
            { "75195072101738541" }, 
            { "2893292468706920608658971875" });
    test_divide({ "36559658086975920957411310021877872011319874893" }, 
            { "2019943414005990417667711981" }, 
            { "18099347651759267700" }, 
            { "1386222149376362340243561193" });
    test_divide({ "5603776877570350325978679279290675695211252157" }, 
            { "6270151941359194639085862409" }, 
            { "893722660946491717" }, 
            { "849174911039614533291085904" });
    test_divide({ "749444980285203105724284887490774834262400000" }, 
            { "7812012161318945645894472197" }, 
            { "95934947976152423" }, 
            { "5312562261633417237854716669" });
    test_divide({ "458612428431662423047527406929285921028369907" }, 
            { "5356606244996548677516748003" }, 
            { "85616229279506788" }, 
            { "1143879966630791183504425543" });
    test_divide({ "6848941728792754496771176800239159992450309375" }, 
            { "7486946442513004020353064000" }, 
            { "914784389254145332" }, 
            { "159269263651299784953061375" });
    test_divide({ "1194246198831112018452497653272890352410622976" }, 
            { "99562226472531255834209411" }, 
            { "11994972803873553969" }, 
            { "56164143310713282454420717" });
    test_divide({ "35387032476818363229992398250619957936938146875" }, 
            { "7714978075688764345865519939" }, 
            { "4586796246165500802" }, 
            { "5058878571946860008886655797" });
    test_divide({ "3206002606902592847693089932372046107245568" }, 
            { "6773439551523230104014159208" }, 
            { "473319734016319" }, 
            { "2519999008171740428371130216" });
}

void test_comparisons(const big_uint & left, const big_uint & right) {
    assert(left < right);
    assert(left <= right);
    assert(right > left);
    assert(right >= left);
    assert(left != right);
    assert(right != left);
    assert(right == right);
    assert(left == left);
}

void test_comparisons() {
    test_comparisons({ 0 }, { 1 });
    test_comparisons({ 0 }, { 2 });
    test_comparisons({ 0 }, { m });
    test_comparisons({ 0 }, { 0, 1 });
    test_comparisons({ 0 }, { 1, 1 });
    test_comparisons({ 0 }, { m, 1 });
    test_comparisons({ 0 }, { 0, 2 });
    test_comparisons({ 0 }, { m, m });
    test_comparisons({ 0 }, { 0, 0, 1 });
    test_comparisons({ 0 }, { 1, 0, 1 });
    test_comparisons({ 0 }, { m, 0, 1 });
    test_comparisons({ 0 }, { 0, 1, 1 });
    test_comparisons({ 0 }, { 0, 0, 2 });
    test_comparisons({ 0 }, { 0, 0, m });
    test_comparisons({ 1 }, { 2 });
    test_comparisons({ 1 }, { m });
    test_comparisons({ 1 }, { 0, 1 });
    test_comparisons({ 1 }, { 1, 1 });
    test_comparisons({ 1 }, { m, 1 });
    test_comparisons({ 1 }, { 0, 2 });
    test_comparisons({ 1 }, { m, m });
    test_comparisons({ 1 }, { 0, 0, 1 });
    test_comparisons({ 1 }, { 1, 0, 1 });
    test_comparisons({ 1 }, { m, 0, 1 });
    test_comparisons({ 1 }, { 0, 1, 1 });
    test_comparisons({ 1 }, { 0, 0, 2 });
    test_comparisons({ 1 }, { 0, 0, m });
    test_comparisons({ 2 }, { m });
    test_comparisons({ 2 }, { 0, 1 });
    test_comparisons({ 2 }, { 1, 1 });
    test_comparisons({ 2 }, { m, 1 });
    test_comparisons({ 2 }, { 0, 2 });
    test_comparisons({ 2 }, { m, m });
    test_comparisons({ 2 }, { 0, 0, 1 });
    test_comparisons({ 2 }, { 1, 0, 1 });
    test_comparisons({ 2 }, { m, 0, 1 });
    test_comparisons({ 2 }, { 0, 1, 1 });
    test_comparisons({ 2 }, { 0, 0, 2 });
    test_comparisons({ 2 }, { 0, 0, m });
    test_comparisons({ m }, { 0, 1 });
    test_comparisons({ m }, { 1, 1 });
    test_comparisons({ m }, { m, 1 });
    test_comparisons({ m }, { 0, 2 });
    test_comparisons({ m }, { m, m });
    test_comparisons({ m }, { 0, 0, 1 });
    test_comparisons({ m }, { 1, 0, 1 });
    test_comparisons({ m }, { m, 0, 1 });
    test_comparisons({ m }, { 0, 1, 1 });
    test_comparisons({ m }, { 0, 0, 2 });
    test_comparisons({ m }, { 0, 0, m });
    test_comparisons({ 0, 1 }, { 1, 1 });
    test_comparisons({ 0, 1 }, { m, 1 });
    test_comparisons({ 0, 1 }, { 0, 2 });
    test_comparisons({ 0, 1 }, { m, m });
    test_comparisons({ 0, 1 }, { 0, 0, 1 });
    test_comparisons({ 0, 1 }, { 1, 0, 1 });
    test_comparisons({ 0, 1 }, { m, 0, 1 });
    test_comparisons({ 0, 1 }, { 0, 1, 1 });
    test_comparisons({ 0, 1 }, { 0, 0, 2 });
    test_comparisons({ 0, 1 }, { 0, 0, m });
    test_comparisons({ 1, 1 }, { m, 1 });
    test_comparisons({ 1, 1 }, { 0, 2 });
    test_comparisons({ 1, 1 }, { m, m });
    test_comparisons({ 1, 1 }, { 0, 0, 1 });
    test_comparisons({ 1, 1 }, { 1, 0, 1 });
    test_comparisons({ 1, 1 }, { m, 0, 1 });
    test_comparisons({ 1, 1 }, { 0, 1, 1 });
    test_comparisons({ 1, 1 }, { 0, 0, 2 });
    test_comparisons({ 1, 1 }, { 0, 0, m });
    test_comparisons({ 0, 2 }, { m, m });
    test_comparisons({ 0, 2 }, { 0, 0, 1 });
    test_comparisons({ 0, 2 }, { 1, 0, 1 });
    test_comparisons({ 0, 2 }, { m, 0, 1 });
    test_comparisons({ 0, 2 }, { 0, 1, 1 });
    test_comparisons({ 0, 2 }, { 0, 0, 2 });
    test_comparisons({ 0, 2 }, { 0, 0, m });
    test_comparisons({ m, 1 }, { m, m });
    test_comparisons({ m, 1 }, { 0, 0, 1 });
    test_comparisons({ m, 1 }, { 1, 0, 1 });
    test_comparisons({ m, 1 }, { m, 0, 1 });
    test_comparisons({ m, 1 }, { 0, 1, 1 });
    test_comparisons({ m, 1 }, { 0, 0, 2 });
    test_comparisons({ m, 1 }, { 0, 0, m });
    test_comparisons({ m, m }, { 0, 0, 1 });
    test_comparisons({ m, m }, { 1, 0, 1 });
    test_comparisons({ m, m }, { m, 0, 1 });
    test_comparisons({ m, m }, { 0, 1, 1 });
    test_comparisons({ m, m }, { 0, 0, 2 });
    test_comparisons({ m, m }, { 0, 0, m });
    test_comparisons({ 0, 0, 1 }, { 1, 0, 1 });
    test_comparisons({ 0, 0, 1 }, { m, 0, 1 });
    test_comparisons({ 0, 0, 1 }, { 0, 1, 1 });
    test_comparisons({ 0, 0, 1 }, { 0, 0, 2 });
    test_comparisons({ 0, 0, 1 }, { 0, 0, m });
}

void test_comparisons_digit_right(const big_uint & left, digit right) {
    assert(left < right);
    assert(left <= right);
    assert(right > left);
    assert(right >= left);
    assert(left != right);
    assert(right != left);
    assert(left == left);
}

void test_comparisons_digit_left(digit left, const big_uint & right) {
    assert(left < right);
    assert(left <= right);
    assert(right > left);
    assert(right >= left);
    assert(left != right);
    assert(right != left);
    assert(left == left);
}

void test_comparisons_digit() {
    test_comparisons_digit_right({ 0 }, 1);
    test_comparisons_digit_right({ 0 }, 2);
    test_comparisons_digit_right({ 0 }, 42);
    test_comparisons_digit_right({ 0 }, m);
    test_comparisons_digit_right({ 1 }, 2);
    test_comparisons_digit_right({ 1 }, 42);
    test_comparisons_digit_right({ 1 }, m);
    test_comparisons_digit_right({ 2 }, 42);
    test_comparisons_digit_right({ 2 }, m);
    test_comparisons_digit_right({ 42 }, m);
    test_comparisons_digit_left(0, { 1 });
    test_comparisons_digit_left(0, { 2 });
    test_comparisons_digit_left(0, { 42 });
    test_comparisons_digit_left(0, { m });
    test_comparisons_digit_left(0, { 0, 1 });
    test_comparisons_digit_left(0, { 1, 1 });
    test_comparisons_digit_left(0, { 2, 1 });
    test_comparisons_digit_left(0, { 42, 1 });
    test_comparisons_digit_left(0, { m, 1 });
    test_comparisons_digit_left(0, { 0, 2 });
    test_comparisons_digit_left(0, { 1, 2 });
    test_comparisons_digit_left(0, { 42, 2 });
    test_comparisons_digit_left(0, { m, 2 });
    test_comparisons_digit_left(0, { 0, 42 });
    test_comparisons_digit_left(0, { 1, 42 });
    test_comparisons_digit_left(0, { 42, 42 });
    test_comparisons_digit_left(0, { m, 42 });
    test_comparisons_digit_left(0, { 0, m });
    test_comparisons_digit_left(0, { 1, m });
    test_comparisons_digit_left(0, { 42, m });
    test_comparisons_digit_left(0, { m, m });
    test_comparisons_digit_left(1, { 2 });
    test_comparisons_digit_left(1, { 42 });
    test_comparisons_digit_left(1, { m });
    test_comparisons_digit_left(1, { 0, 1 });
    test_comparisons_digit_left(1, { 1, 1 });
    test_comparisons_digit_left(1, { 2, 1 });
    test_comparisons_digit_left(1, { 42, 1 });
    test_comparisons_digit_left(1, { m, 1 });
    test_comparisons_digit_left(1, { 0, 2 });
    test_comparisons_digit_left(1, { 1, 2 });
    test_comparisons_digit_left(1, { 42, 2 });
    test_comparisons_digit_left(1, { m, 2 });
    test_comparisons_digit_left(1, { 0, 42 });
    test_comparisons_digit_left(1, { 1, 42 });
    test_comparisons_digit_left(1, { 42, 42 });
    test_comparisons_digit_left(1, { m, 42 });
    test_comparisons_digit_left(1, { 0, m });
    test_comparisons_digit_left(1, { 1, m });
    test_comparisons_digit_left(1, { 42, m });
    test_comparisons_digit_left(1, { m, m });
    test_comparisons_digit_left(2, { 42 });
    test_comparisons_digit_left(2, { m });
    test_comparisons_digit_left(2, { 0, 1 });
    test_comparisons_digit_left(2, { 1, 1 });
    test_comparisons_digit_left(2, { 2, 1 });
    test_comparisons_digit_left(2, { 42, 1 });
    test_comparisons_digit_left(2, { m, 1 });
    test_comparisons_digit_left(2, { 0, 2 });
    test_comparisons_digit_left(2, { 1, 2 });
    test_comparisons_digit_left(2, { 42, 2 });
    test_comparisons_digit_left(2, { m, 2 });
    test_comparisons_digit_left(2, { 0, 42 });
    test_comparisons_digit_left(2, { 1, 42 });
    test_comparisons_digit_left(2, { 42, 42 });
    test_comparisons_digit_left(2, { m, 42 });
    test_comparisons_digit_left(2, { 0, m });
    test_comparisons_digit_left(2, { 1, m });
    test_comparisons_digit_left(2, { 42, m });
    test_comparisons_digit_left(2, { m, m });
    test_comparisons_digit_left(42, { m });
    test_comparisons_digit_left(42, { 0, 1 });
    test_comparisons_digit_left(42, { 1, 1 });
    test_comparisons_digit_left(42, { 2, 1 });
    test_comparisons_digit_left(42, { 42, 1 });
    test_comparisons_digit_left(42, { m, 1 });
    test_comparisons_digit_left(42, { 0, 2 });
    test_comparisons_digit_left(42, { 1, 2 });
    test_comparisons_digit_left(42, { 42, 2 });
    test_comparisons_digit_left(42, { m, 2 });
    test_comparisons_digit_left(42, { 0, 42 });
    test_comparisons_digit_left(42, { 1, 42 });
    test_comparisons_digit_left(42, { 42, 42 });
    test_comparisons_digit_left(42, { m, 42 });
    test_comparisons_digit_left(42, { 0, m });
    test_comparisons_digit_left(42, { 1, m });
    test_comparisons_digit_left(42, { 42, m });
    test_comparisons_digit_left(42, { m, m });
    test_comparisons_digit_left(m, { 0, 1 });
    test_comparisons_digit_left(m, { 1, 1 });
    test_comparisons_digit_left(m, { 2, 1 });
    test_comparisons_digit_left(m, { 42, 1 });
    test_comparisons_digit_left(m, { m, 1 });
    test_comparisons_digit_left(m, { 0, 2 });
    test_comparisons_digit_left(m, { 1, 2 });
    test_comparisons_digit_left(m, { 42, 2 });
    test_comparisons_digit_left(m, { m, 2 });
    test_comparisons_digit_left(m, { 0, 42 });
    test_comparisons_digit_left(m, { 1, 42 });
    test_comparisons_digit_left(m, { 42, 42 });
    test_comparisons_digit_left(m, { m, 42 });
    test_comparisons_digit_left(m, { 0, m });
    test_comparisons_digit_left(m, { 1, m });
    test_comparisons_digit_left(m, { 42, m });
    test_comparisons_digit_left(m, { m, m });
}

void test_pow(const big_uint & base, digit exponent, const big_uint & power) {
    big_uint t = base.pow(exponent);
    assert(t == power);
    assert(t.satisfies_invariant());
}

void test_pow() {
    test_pow({ 0 }, 0, { 1 });
    test_pow({ 1 }, 0, { 1 });
    test_pow({ 2 }, 0, { 1 });
    test_pow({ 42 }, 0, { 1 });
    test_pow({ m }, 0, { 1 });
    test_pow({ 0, 1 }, 0, { 1 });
    test_pow({ 1, 1 }, 0, { 1 });
    test_pow({ 2, 1 }, 0, { 1 });
    test_pow({ 42, 1 }, 0, { 1 });
    test_pow({ m, 1 }, 0, { 1 });
    test_pow({ 0, m }, 0, { 1 });
    test_pow({ 1, m }, 0, { 1 });
    test_pow({ 2, m }, 0, { 1 });
    test_pow({ 42, m }, 0, { 1 });
    test_pow({ m, m }, 0, { 1 });
    test_pow({ 432, 432, 32, 42, 3214, 32 }, 0, { 1 });
    test_pow({ 0 }, 1, { 0 });
    test_pow({ 1 }, 1, { 1 });
    test_pow({ 2 }, 1, { 2 });
    test_pow({ 42 }, 1, { 42 });
    test_pow({ m }, 1, { m });
    test_pow({ 0, 1 }, 1, { 0, 1 });
    test_pow({ 1, 1 }, 1, { 1, 1 });
    test_pow({ 2, 1 }, 1, { 2, 1 });
    test_pow({ 42, 1 }, 1, { 42, 1 });
    test_pow({ m, 1 }, 1, { m, 1 });
    test_pow({ 0, m }, 1, { 0, m });
    test_pow({ 1, m }, 1, { 1, m });
    test_pow({ 2, m }, 1, { 2, m });
    test_pow({ 42, m }, 1, { 42, m });
    test_pow({ m, m }, 1, { m, m });
    test_pow({ 432, 432, 32, 42, 3214, 32 }, 1, { 432, 432, 32, 42, 3214, 32 });
    test_pow({ 0 }, 2, { 0 });
    test_pow({ 1 }, 2, { 1 });
    test_pow({ 2 }, 2, { 4 });
    test_pow({ 42 }, 2, { 1764 });
    test_pow({ m }, 2, { 1, m - 1 });
    test_pow({ 0, 1 }, 2, { "18446744073709551616" });
    test_pow({ 1, 1 }, 2, { "18446744082299486209" });
    test_pow({ 2, 1 }, 2, { "18446744090889420804" });
    test_pow({ 42, 1 }, 2, { "18446744434486806244" });
    test_pow({ m, 1 }, 2, { "73786976277658337281" });
    test_pow({ 0, m }, 2, { "340282366762482138453292676318389862400" });
    test_pow({ 1, m }, 2, { "340282366762482138490186164457219031041" });
    test_pow({ 2, m }, 2, { "340282366762482138527079652596048199684" });
    test_pow({ 42, m }, 2, { "340282366762482140002819178149214947044" });
    test_pow({ m, m }, 2, { "340282366920938463426481119284349108225" });
    test_pow({ "3143271849327891473289789789374282" }, 2, 
            { "9880157918777182876145376146391788801255400268620362724753083015524" });
    test_pow({ "4738291477084372891789473829478932" }, 2, 
            { "22451406121810408237118933033938067953672353960296953800970631860624" });
    test_pow({ "7832748932748732984789328978978924" }, 2, 
            { "61351955843476415598484178692699172363218502316314342645877636197776" });
    test_pow({ "7832748932748732984789328978978924" }, 3,
            { "48055446665503728646756430172310507262823159221860175454456200861257"
            "9264593367350696903455090199673024" });
    test_pow({ "7832748932748732984789328978978924" }, 4,
            { "37640624858198798981714624695996812167887317208197410485913958181064" 
            "81168875242192851932685244873236552032333091715547210686714187346176" });
    test_pow({ "7832748932748732984789328978978924" }, 5,
            { "29482956418605207156712251373059597153824942108112400032339679726656"
            "45136058269078956482802131301333301516384872346107400036428040047646"
            "0966927514522819522633421795994624" });
    test_pow({ "7832748932748732984789328978978924" }, 6,
            { "23093259542210734324609262533320917475849421542615409832514098315312"
            "22100837534767864572262286162529648450618120466513746086210888858904"
            "86292357949822502387270896203271156294892685348727776853636513304576" });
    test_pow({ "7832748932748732984789328978978924" }, 7,
            { "18088370403294062334557303846862144133200491268479281763204663082973"
            "82434279258115610102744787272625650576199856503998552392704568689649"
            "18352036357484972031461188848177029741027438166989380281999867222073"
            "9249387133844193637646953496756224" });
    test_pow({ "7832748932748732984789328978978924" }, 42,
            { "35026492989601039592147227325755225666528894742602967040639035962137"
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
            "3994015881285443648764403952305187580493448759701619748714315776" });
}

int main() {
    cout << "big_uint_tests.cpp\n";
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
