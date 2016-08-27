#include "big_int.hpp"
#include "assert.hpp"

#include <cassert>

using namespace std;
using namespace big;

void test_constructors(const big_int & left, const big_int & right) {
    assert(left == right);
    assert(left.satisfies_invariant());
    assert(right.satisfies_invariant());
}

void test_constructors() {
    test_constructors({}, { 0 });
    test_constructors({ 0 }, { "0" });
    test_constructors({ 100 }, { "100" });
    test_constructors({ -100 }, { "-100" });
}

void test_preincrement(big_int prev, const big_int & next) {
    ++prev;
    assert(prev == next);
    assert(prev.satisfies_invariant());
}

void test_postincrement(big_int prev, const big_int & next) {
    prev++;
    assert(prev == next);
    assert(prev.satisfies_invariant());
}

void test_predecrement(const big_int & prev, big_int next) {
    --next;
    assert(prev == next);
    assert(next.satisfies_invariant());
}

void test_postdecrement(const big_int & prev, big_int next) {
    next--;
    assert(prev == next);
    assert(next.satisfies_invariant());
}

void test_increment_and_decrement(const big_int & prev, const big_int & next) {
    test_preincrement(prev, next);
    test_postincrement(prev, next);
    test_predecrement(prev, next);
    test_postdecrement(prev, next);
}

void test_increment_and_decrement() {
    test_increment_and_decrement(0, 1);
    test_increment_and_decrement(1, 2);
    test_increment_and_decrement(-1, 0);
    test_increment_and_decrement(-2, -1);
}

void test_addition_concrete(const big_int & augend, const big_int & audend, 
        const big_int & sum) {
    big_int t = augend + audend;
    assert(t == sum);
    assert(t.satisfies_invariant());
    t = augend;
    t += audend;
    assert(t == sum);
    assert(t.satisfies_invariant());
}

void test_addition(const big_int & augend, const big_int & audend, 
        const big_int & sum) {
    test_addition_concrete(augend, audend, sum);
    test_addition_concrete(audend, augend, sum);
}

void test_subtraction_concrete(const big_int & minuend, const big_int & subtrahend, 
        const big_int & difference) {
    big_int t = minuend - subtrahend;
    assert(t == difference);
    assert(t.satisfies_invariant());
    t = minuend;
    t -= subtrahend;
    assert(t == difference);
    assert(t.satisfies_invariant());
}

void test_subtraction(const big_int & augend, const big_int & audend, 
        const big_int & sum) {
    test_subtraction_concrete(sum, augend, audend);
    test_subtraction_concrete(sum, audend, augend);
}

void test_addition_and_subtraction(const big_int & augend, const big_int & audend, 
        const big_int & sum) {
    test_addition(augend, audend, sum);
    test_addition(sum, -audend, augend);
    test_addition(sum, -augend, audend);
    test_subtraction(augend, audend, sum);
    test_subtraction(sum, -audend, augend);
    test_subtraction(sum, -augend, audend);
}

void test_addition_and_subtraction() {
    test_addition_and_subtraction(0, 0, 0);
    test_addition_and_subtraction(0, 1, 1);
    test_addition_and_subtraction(0, 100, 100);
    test_addition_and_subtraction(1, 1, 2);
    test_addition_and_subtraction(1, 2, 3);
    test_addition_and_subtraction(1, 100, 101);
    test_addition_and_subtraction(2, 2, 4);
    test_addition_and_subtraction(2, 100, 102);
    test_addition_and_subtraction(100, 100, 200);
}

int main() {
    cout << "big_int_tests.cpp\n";
    test_constructors();
    test_increment_and_decrement();
    cout << "OK!\n";
    return 0;
}
