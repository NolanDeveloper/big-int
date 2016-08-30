#include <iostream>
#include <chrono>
#include <iomanip>

#include "big_uint.hpp"

using namespace std;
using namespace std::chrono;
using namespace big;

static high_resolution_clock::time_point last_start;

void start_measurement() {
    last_start = high_resolution_clock::now();
}

high_resolution_clock::duration elapsed() {
    return high_resolution_clock::now() - last_start;
}

big_uint get_rand_big_int(size_t length) {
    deque<digit> digits(length);
    for (int i = 0; i < length - 1; ++i) {
        digits.push_back(rand());
    }
    digits.push_back(1);
    return big_uint(digits);
}

template <typename F, unsigned N = 100>
long measure_multiplication(size_t length, F method) {
    big_uint lhs = get_rand_big_int(length);
    big_uint rhs = get_rand_big_int(length);
    milliseconds total_duration{ 0 };
    for (int i = 0; i < N; ++i) {
        start_measurement();
        method(lhs, rhs);
        total_duration += duration_cast<milliseconds>(elapsed());
    }
    return total_duration.count() / N;
}

int main() {
    size_t begin_length = 0;
    size_t end_length = 1024;
    while (begin_length + 1 != end_length) {
        size_t length = (begin_length + end_length) / 2;
        long karatsuba_time = measure_multiplication(length, big_uint::karatsuba_multiply);
        long school_time = measure_multiplication(length, big_uint::school_multiply);
        if (karatsuba_time < school_time) {
            end_length = length;
        } else {
            begin_length = length;
        }
    }
    cout << "karatsuba_threshold: " << begin_length << '\n';
    return 0;
}
