#include "big_uint.hpp"

#include <iterator>
#include <algorithm>
#include <sstream>
#include <utility>
#include <cassert>

namespace big {

using namespace std;

big_uint::big_uint() : _digits(1, 0) { }

big_uint::big_uint(digit d) : _digits(1, d) { }

big_uint::big_uint(initializer_list<digit> digits) : _digits(digits) {
    assert(satisfies_invariant());
}

big_uint::big_uint(deque<digit> digits) : _digits(digits) { 
    while (!_digits.empty() && !_digits.back()) {
        _digits.pop_back();
    }
    if (_digits.empty()) _digits.resize(1);
}

big_uint::big_uint(const std::string & num) {
    istringstream iss(num);
    iss >> *this;
}

big_uint & big_uint::operator=(digit d) {
    _digits.resize(1);
    _digits[0] = d;
    return *this;
}

void big_uint::add_with_shift(const big_uint & x, size_t s) {
    if (_digits.size() <= s) _digits.resize(s + 1);
    auto prev  = _digits.begin() + s;
    auto it    = prev + 1;
    auto end   = _digits.end();
    auto _prev = x._digits.begin();
    auto _it   = _prev + 1;
    auto _end  = x._digits.end();
    long_digit t = static_cast<long_digit>(*prev) + *_prev;
    *prev = t;
    while (it != end && _it != _end) {
        t = static_cast<long_digit>(*it) + *_it + (t >> 8 * sizeof(digit));
        *it   = t;
        prev  = it;
        _prev = _it;
        ++it; ++_it;
    }
    if (_it == _end) { 
        if (t >> 8 * sizeof(digit)) {
            if (it == end) {
                _digits.push_back(1);
            } else {
                ++*it; prev = it; ++it;
                while (it != end && *prev == 0) {
                    ++*it; prev = it; ++it;
                }
                if (it == end && *prev == 0) _digits.push_back(1);
            }
        }
    } else { // if (it == end)
        _digits.push_back(*_it + (t >> 8 * sizeof(digit)));
        _prev = _it; ++_it;
        while (_it != _end) {
            _digits.push_back(*_it + (_digits.back() < *_prev));
            _prev = _it;
            ++_it;
        }
        if (_digits.back() < *_prev) _digits.push_back(1);
    }
}

const std::deque<digit> big_uint::digits() const {
    return _digits;
}

big_uint & big_uint::operator++() {
    auto it = _digits.begin();
    auto end = _digits.end();
    do { ++*it; } while (0 == *it && ++it != end);
    if (it == end) _digits.push_back(1);
    return *this;
}

big_uint & big_uint::operator--() {
    assert(_digits.size() > 1 || _digits[0] != 0);
    if (*this == 1u) {
        _digits[0] = 0;
        return *this;
    }
    auto it = _digits.begin();
    auto end = _digits.end();
    do { --*it; } while (numeric_limits<digit>::max() == *it && ++it != end);
    if (0 == *it && ++it == end) _digits.pop_back();
    return *this;
}

big_uint big_uint::operator++(int) {
    big_uint t = *this;
    ++*this;
    return t;
}

big_uint big_uint::operator--(int) {
    big_uint t = *this;
    --*this;
    return t;
}

big_uint operator+(const big_uint & lhs, digit rhs) {
    if (rhs == 0u) 
        return lhs;
    deque<digit> res;
    auto prev = lhs._digits.begin();
    auto it   = prev + 1;
    auto end  = lhs._digits.end();
    res.push_back(*prev + rhs);
    if (res.back() < rhs) {
        if (it == end) {
            res.push_back(1);
            return big_uint(res);
        } 
        res.push_back(*it + 1);
        prev = it; 
        ++it;
    }
    while (*prev == numeric_limits<digit>::max() && it != end) {
        res.push_back(*it + 1);
        prev = it; 
        ++it;
    }
    if (*prev == numeric_limits<digit>::max() && it == end) 
        res.push_back(1);
    else
        copy(it, end, back_inserter(res));
    return big_uint(res);
}

big_uint operator-(const big_uint & lhs, digit rhs) {
    assert(lhs >= rhs);
    deque<digit> res;
    auto prev = lhs._digits.begin();
    auto it   = prev + 1;
    auto end  = lhs._digits.end();
    res.push_back(*prev - rhs);
    if (res.back() > *prev) {
        res.push_back(*it - 1);
        prev = it; 
        ++it;
    }
    while (res.back() == numeric_limits<digit>::max() && it != end) {
        res.push_back(*it - 1);
        prev = it; 
        ++it;
    }
    if (res.size() != 1 && res.back() == 0) 
        res.pop_back();
    return big_uint(res);
}

big_uint operator*(const big_uint & lhs, digit rhs) {
    if (rhs == 0u || lhs == 0u) 
        return { 0u };
    deque<digit> res;
    digit carry = 0;
    long_digit ld;
    auto it = lhs._digits.begin();
    auto end = lhs._digits.end();
    while (it != end) {
        ld = (long_digit) rhs * *it + carry;
        res.push_back(ld);
        carry = ld >> (sizeof(digit) * 8);
        ++it;
    }
    if (carry) 
        res.push_back(carry);
    return big_uint(res);
}

big_uint operator/(const big_uint & lhs, digit rhs) {
    assert(rhs != 0);
    return big_uint::div(lhs, rhs);
}

big_uint operator%(const big_uint & lhs, digit rhs) {
    assert(rhs != 0);
    digit rem;
    big_uint::div(lhs, rhs, rem);
    return { rem };
}

big_uint operator+(digit lhs, const big_uint & rhs) {
    return rhs + lhs;
}

big_uint operator-(digit lhs, const big_uint & rhs) {
    assert(lhs >= rhs);
    return { lhs - rhs._digits[0] };
}

big_uint operator*(digit lhs, const big_uint & rhs) {
    return rhs * lhs;
}

big_uint operator/(digit lhs, const big_uint & rhs) {
    assert(rhs != 0u);
    if (rhs > lhs) return { 0u };
    return { lhs / rhs._digits[0] };
}

big_uint operator%(digit lhs, const big_uint & rhs) {
    assert(rhs != 0u);
    if (rhs > lhs) return { lhs };
    return { lhs % rhs._digits[0] };
}

big_uint & big_uint::operator+=(digit d) {
    if (d == 0) return *this;
    if (*this == 0u) {
        _digits[0] = d;
        return *this;
    }
    auto prev = _digits.begin();
    auto it   = prev + 1;
    auto end  = _digits.end();
    *prev += d;
    if (*prev < d) {
        if (it == end) {
            _digits.push_back(1);
            return *this;
        } 
        ++*it; 
        prev = it; 
        ++it;
    }
    while (*prev == 0 && it != end) {
        ++*it; 
        prev = it; 
        ++it;
    }
    if (*prev == 0 && it == end) 
        _digits.push_back(1);
    return *this;
}

big_uint & big_uint::operator-=(digit d) {
    assert(*this >= d);
    auto prev = _digits.begin();
    auto it   = prev + 1;
    auto end  = _digits.end();
    digit t = *prev;
    *prev -= d;
    if (*prev > t) {
        --*it; prev = it; ++it;
    }
    while (*prev == numeric_limits<digit>::max() && it != end) {
        --*it; prev = it; ++it;
    }
    if (_digits.size() != 1 && _digits.back() == 0) _digits.pop_back();
    return *this;
}

big_uint & big_uint::operator*=(digit d) {
    if (*this == 0u) return *this;
    if (d == 0) {
        _digits.resize(1);
        _digits[0] = 0;
        return *this;
    }
    digit carry = 0;
    long_digit ld;
    auto it = _digits.begin();
    auto end = _digits.end();
    while (it != end) {
        ld = (long_digit) d * *it + carry;
        *it = ld;
        carry = ld >> (sizeof(digit) * 8);
        ++it;
    }
    if (carry) _digits.push_back(carry);
    return *this;
}

big_uint & big_uint::operator/=(digit d) {
    assert(d != 0);
    return *this = div(*this, d);
}

big_uint & big_uint::operator%=(digit d) {
    assert(d != 0);
    digit rem;
    div(*this, d, rem); 
    _digits.clear();
    _digits.push_back(rem);
    return *this;
}

big_uint big_uint::div(const big_uint & dividend, digit divisor, digit & reminder) {
    assert(divisor != 0);
    if (dividend < divisor) {
        reminder = dividend._digits[0];
        return { 0u };
    }
    auto it  = dividend._digits.rbegin();
    auto end = dividend._digits.rend();
    deque<digit> quot;
    long_digit x = *it;
    ++it;
    if (x < divisor) {
        x <<= sizeof(digit) * 8;
        x |= *it;
        ++it;
    }
    while (true) {
        quot.push_front(x / divisor);
        x = x % divisor;
        if (it == end) break;
        x <<= sizeof(digit) * 8;
        x |= *it;
        ++it;
    }
    reminder = x;
    return big_uint(quot);
}

big_uint big_uint::div(const big_uint & dividend, digit divisor) {
    digit reminder;
    return div(dividend, divisor, reminder);
}

big_uint & big_uint::operator+=(const big_uint & x) {
    add_with_shift(x, 0);
    return *this;
}

big_uint & big_uint::operator-=(const big_uint & x) { 
    assert(*this >= x);
    auto prev  = _digits.begin();
    auto it    = prev + 1;
    auto end   = _digits.end();
    auto _prev = x._digits.begin();
    auto _it   = _prev + 1;
    auto _end  = x._digits.end();
    auto msd   = prev; // position of last non zero digit
    bool carry = *prev < *_prev;
    *prev -= *_prev;
    while (_it != _end) {
        if (*it < *_it) {
            if (carry) --*it;
            carry = true;
        } else if (carry) {
            carry = *it == *_it;   
            --*it;
        }
        *it -= *_it;
        if (*it) msd = it;
        prev = it; 
        ++it;      
        _prev = _it;
        ++_it;
    }
    while (carry) {
        carry = *it == 0;
        if (--*it) msd = it;
        ++it;
    }
    if (it != end) return *this;
    ++msd;
    _digits.erase(msd, end);
    return *this;
}

big_uint big_uint::school_multiply(const big_uint & lhs, const big_uint & rhs) {
    if (lhs == 1u) return rhs;
    if (lhs == 0u || rhs == 0u) return { 0u };
    big_uint res;
    for (size_t s = 0; s < lhs._digits.size(); ++s)
        res.add_with_shift(rhs * lhs._digits[s], s);
    return res;
}

/*
 * (a + bx)(c + dx) = ac + ((a + b)(c + d) - ac - bd) * x + db * x^2
 */
big_uint big_uint::karatsuba_multiply(const big_uint & lhs, const big_uint & rhs) {
    if (lhs._digits.size() == 1) return lhs._digits[0] * rhs;
    if (rhs._digits.size() == 1) return rhs._digits[0] * lhs;
    auto digit_size = max(lhs._digits.size(), rhs._digits.size()) / 2;
    auto lhs_begin = lhs._digits.begin();
    auto lhs_mid = lhs_begin + digit_size;
    auto lhs_end = lhs._digits.end();
    big_uint a{ deque<digit>(lhs_begin, lhs_mid) };
    big_uint b{ deque<digit>(lhs_mid, lhs_end) };
    auto rhs_begin = rhs._digits.begin();
    auto rhs_mid = rhs_begin + digit_size;
    auto rhs_end = rhs._digits.end();
    big_uint c{ deque<digit>(rhs_begin, rhs_mid) };
    big_uint d{ deque<digit>(rhs_mid, rhs_end) };
    big_uint ac = a * c;
    big_uint bd = b * d;
    big_uint result = ac;
    result.add_with_shift((move(a) + b) * (move(c) + d) - ac - bd, digit_size);
    result.add_with_shift(bd, 2 * digit_size);
    return result;
}

big_uint & big_uint::operator*=(const big_uint & x) {
    size_t max_width = max(_digits.size(), x._digits.size());
    if (max_width < KARATSUBA_THRESHOLD) {
        return *this = school_multiply(*this, x);
    } else {
        return *this = karatsuba_multiply(*this, x);
    }
}

big_uint & big_uint::operator/=(const big_uint & x) {
    return *this = div(*this, x);
}

big_uint & big_uint::operator%=(const big_uint & x) {
    big_uint rem;
    div(*this, x, rem);
    return *this = rem;
}

big_uint big_uint::div(const big_uint & dividend, const big_uint & divisor, 
                       big_uint & reminder) {
    assert(divisor != 0u);
    if (dividend < divisor) {
        reminder = dividend;
        return { };
    }
    if (divisor == 1u) {
        reminder = { };
        return dividend;
    }
    auto it = dividend._digits.rbegin();
    auto end = dividend._digits.rend();
    big_uint quot{ 1u };
    reminder = { };
    int i = 8 * sizeof(digit) - 1;
    while (true) {
        for (; i >= 0 && reminder < divisor; --i) {
            reminder *= 2;
            if ((*it >> i) & 0x1)
                ++reminder;
        }
        if (reminder >= divisor)
            break;
        ++it;
        i = 8 * sizeof(digit) - 1;
    }
    reminder -= divisor;
    while (it != end) {
        for (; i >= 0; --i) {
            reminder *= 2;
            if ((*it >> i) & 0x1)
                ++reminder;
            quot *= 2;
            if (reminder >= divisor) {
                ++quot;
                reminder -= divisor;
            }
        }
        ++it;
        i = 8 * sizeof(digit) - 1;
    }
    return quot;
}

big_uint big_uint::div(const big_uint & dividend, const big_uint & divisor) {
    big_uint reminder;
    return div(dividend, divisor, reminder);
}

bool big_uint::operator==(const big_uint & rhs) const {
    return _digits == rhs._digits;
}

bool big_uint::operator!=(const big_uint & rhs) const {
    return !(*this == rhs);
}

bool big_uint::operator<(const big_uint & rhs) const {
    return _digits.size() < rhs._digits.size() ||
        (_digits.size() == rhs._digits.size() && 
        lexicographical_compare(_digits.rbegin(), _digits.rend(), 
                                rhs._digits.rbegin(), rhs._digits.rend()));
}

bool big_uint::operator>(const big_uint & rhs) const {
    return rhs._digits.size() < _digits.size() ||
        (rhs._digits.size() == _digits.size() && 
        lexicographical_compare(rhs._digits.rbegin(), rhs._digits.rend(), 
                                _digits.rbegin(), _digits.rend()));
}

bool big_uint::operator<=(const big_uint & rhs) const {
    return !(*this > rhs);
}

bool big_uint::operator>=(const big_uint & rhs) const {
    return !(*this < rhs);
}

bool operator<(long_digit lhs, const big_uint & rhs) {
    if (rhs._digits.size() > 2) return true;
    long_digit right = (rhs._digits.size() == 1 ? 0 : rhs._digits[1]);
    right <<= 8 * sizeof(digit);
    right |= rhs._digits[0];
    return lhs < right;
}

bool operator>(long_digit lhs, const big_uint & rhs) {
    if (rhs._digits.size() > 2) return false;
    long_digit right = (rhs._digits.size() == 1 ? 0 : rhs._digits[1]);
    right <<= 8 * sizeof(digit);
    right |= rhs._digits[0];
    return lhs > right;
}

bool operator<=(long_digit lhs, const big_uint & rhs) {
    if (rhs._digits.size() > 2) return false;
    long_digit right = (rhs._digits.size() == 1 ? 0 : rhs._digits[1]);
    right <<= 8 * sizeof(digit);
    right |= rhs._digits[0];
    return lhs <= right;
}

bool operator>=(long_digit lhs, const big_uint & rhs) {
    if (rhs._digits.size() > 2) return false;
    long_digit right = (rhs._digits.size() == 1 ? 0 : rhs._digits[1]);
    right <<= 8 * sizeof(digit);
    right |= rhs._digits[0];
    return lhs >= right;
}

bool operator==(long_digit lhs, const big_uint & rhs) {
    if (rhs._digits.size() > 2) return false;
    long_digit right = (rhs._digits.size() == 1 ? 0 : rhs._digits[1]);
    right <<= 8 * sizeof(digit);
    right |= rhs._digits[0];
    return lhs == right;
}

bool operator!=(long_digit lhs, const big_uint & rhs) {
    return !(lhs == rhs);
}

bool operator<(const big_uint & lhs, long_digit rhs) {
    return rhs > lhs;
}

bool operator>(const big_uint & lhs, long_digit rhs) {
    return rhs < lhs;
}

bool operator<=(const big_uint & lhs, long_digit rhs) {
    return rhs >= lhs;
}

bool operator>=(const big_uint & lhs, long_digit rhs) {
    return rhs <= lhs;
}

bool operator==(const big_uint & lhs, long_digit rhs) {
    return lhs == rhs;
}

bool operator!=(const big_uint & lhs, long_digit rhs) {
    return !(lhs == rhs);
}

bool operator<(digit lhs, const big_uint & rhs) {
    return rhs._digits.size() > 1 || lhs < rhs._digits[0];
}

bool operator>(digit lhs, const big_uint & rhs) {
    return rhs._digits.size() == 1 && lhs > rhs._digits[0];
}

bool operator<=(digit lhs, const big_uint & rhs) {
    return rhs._digits.size() > 1 || lhs <= rhs._digits[0];
}

bool operator>=(digit lhs, const big_uint & rhs) {
    return rhs._digits.size() == 1 && lhs >= rhs._digits[0];
}

bool operator==(digit lhs, const big_uint & rhs) {
    return rhs._digits.size() == 1 && rhs._digits[0] == lhs;
}

bool operator!=(digit lhs, const big_uint & rhs) {
    return !(lhs == rhs);
}

bool operator<(const big_uint & lhs, digit rhs) {
    return rhs > lhs;
}

bool operator>(const big_uint & lhs, digit rhs) {
    return rhs < lhs;
}

bool operator<=(const big_uint & lhs, digit rhs) {
    return rhs >= lhs;
}

bool operator>=(const big_uint & lhs, digit rhs) {
    return rhs <= lhs;
}

bool operator==(const big_uint & lhs, digit rhs) {
    return rhs == lhs;
}

bool operator!=(const big_uint & lhs, digit rhs) {
    return !(lhs == rhs);
}

big_uint big_uint::pow(digit e) const {
    if (e == 0)
        return { 1 };
    if (*this == 0u) 
        return { 0 };
    big_uint x{ *this };
    big_uint y{ 1 };
    while (e > 1) {
        if (e & 1) {
            y *= x;
            x *= x;
        } else
            x *= x;
        e >>= 1;
    }
    return x * y;
}

bool big_uint::satisfies_invariant() const {
    return _digits.size() == 1 ||
        (_digits.size() > 1 && _digits.back() != 0);
}

ostream & operator<<(ostream & os, big_uint x) {
    if (x == 0u)
        return os << '0';
    vector<char> result;
    digit rem;
    while (x != 0u) {
        x = big_uint::div(x, 10, rem);
        result.push_back('0' + rem);
    }
    copy(result.rbegin(), result.rend(), ostream_iterator<char>(os));
    return os;
}

istream & operator>>(istream & is, big_uint & x) {
    x = { 0u };
    for (istreambuf_iterator<char> it{ is }, end; it != end && isdigit(*it); ++it) {
        x *= 10;
        x += *it - '0';
    }
    return is;
}

}
