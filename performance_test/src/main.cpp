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

big_uint t;

template <unsigned N = 200>
long measure_multiplication(size_t length, size_t threshold) {
    big_uint::set_karatsuba_threshold(threshold);
    big_uint lhs = get_rand_big_int(length);
    big_uint rhs = get_rand_big_int(length);
    milliseconds total_duration{ 0 };
    for (int i = 0; i < N; ++i) {
        start_measurement();
        t = lhs * rhs;
        total_duration += duration_cast<milliseconds>(elapsed());
    }
    return total_duration.count() / N;
}

int main() {
    size_t low_threshold = 71;
    size_t high_threshold = 320;
    cout << setw(10) << "threshold,";
    cout << setw(15) << "time(ms)\n";
    for (int i = 0; i < 100; ++i) {
        size_t threshold = low_threshold + (high_threshold + low_threshold) * i / 100;
        long time = measure_multiplication(1024, threshold);
        cout << setw(9) << threshold << ',';
        cout << setw(15) << time << '\n';
    }
    if (t > 10u) {
        return 0;
    } else {
        return 1;
    }
}
