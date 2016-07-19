#include "big_uint.hpp"

#include <iterator>
#include <algorithm>
#include <sstream>
#include <utility>
#include <cassert>

namespace big {

using namespace std;

big_uint::big_uint(deque<digit> digits) : _digits(digits) { 
    if (!satisfies_invariant())
        throw logic_error("Digits violate invariant");
}

big_uint::big_uint() : _digits(1, 0) { }

big_uint::big_uint(digit d) : _digits(1, d) { }

big_uint::big_uint(initializer_list<digit> digits) : _digits(digits) {
    if (!satisfies_invariant())
        throw logic_error("Digits violate invariant");
}

big_uint::big_uint(const std::string & num) : big_uint(num.cbegin(), num.cend()) { }

big_uint::big_uint(string::const_iterator begin, string::const_iterator end)
        : _digits(1, 0) {
    if (begin == end) throw logic_error("Empty input");
    auto it = begin;
    while (it != end) {
        if (!isdigit(*it)) {
            ostringstream oss;
            oss << "Input is not unsigned integer: \"";
            copy(begin, end, ostream_iterator<char>(oss));
            oss << '\"';
            throw logic_error(oss.str().c_str());
        }
        *this *= 10;
        *this += *it - '0';
        ++it;
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
    if (_digits.size() == 1 && _digits[0] == 0)
        throw logic_error("Attempt to decrement zero.");
    if (*this == 1) {
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

big_uint & big_uint::operator+=(digit d) {
    if (*this == 0) {
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
    if (*this < d)
        throw logic_error("Minuend must be greater than subtrahend.");
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
    if (*this == 0) return *this;
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
    if (d == 0) throw logic_error("Division by zero.");
    return *this = div(*this, d);
}

big_uint & big_uint::operator%=(digit d) {
    if (d == 0) throw logic_error("Division by zero.");
    digit rem;
    div(*this, d, rem); 
    _digits.clear();
    _digits.push_back(rem);
    return *this;
}

big_uint big_uint::div(const big_uint & num, digit denom, digit & rem) {
    if (denom == 0) throw logic_error("Division by zero.");
    if (num < denom) {
        rem = num._digits[0];
        return 0;
    }
    auto it  = num._digits.rbegin();
    auto end = num._digits.rend();
    deque<digit> quot;
    long_digit x = *it;
    ++it;
    if (x < denom) {
        x <<= sizeof(digit) * 8;
        x |= *it;
        ++it;
    }
    while (true) {
        quot.push_front(x / denom);
        x = x % denom;
        if (it == end) break;
        x <<= sizeof(digit) * 8;
        x |= *it;
        ++it;
    }
    rem = x;
    return { move(quot) };
}

big_uint big_uint::div(const big_uint & num, digit denom) {
    digit rem;
    return div(num, denom, rem);
}

big_uint & big_uint::operator+=(const big_uint & x) {
    auto prev  = _digits.begin();
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
    return *this;
}

big_uint & big_uint::operator-=(const big_uint & x) { 
    if (*this < x) {
        cout << *this << "\n" << x << endl;
        throw logic_error("Minuend must be greater than subtrahend.");
    }
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

void big_uint::add_with_shift(const big_uint & x, size_t s) {
    if (x == 0) return;
    if (_digits.size() < s) _digits.resize(s);
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

big_uint & big_uint::operator*=(const big_uint & x) {
    if (*this == 1) return *this = x;
    big_uint res;
    big_uint t;
    size_t s = 0;
    for (digit d : x._digits) {
        t = *this;
        t *= d;
        res.add_with_shift(t, s);
        ++s;
    }
    return *this = res;
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
    if (divisor == 0) throw logic_error("Division by zero.");
    if (dividend < divisor) {
        reminder = dividend;
        return 0;
    }
    if (divisor == 1) {
        reminder = 0;
        return dividend;
    }
    auto it = dividend._digits.rbegin();
    auto end = dividend._digits.rend();
    big_uint quot = 1;
    reminder = 0;
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

bool big_uint::satisfies_invariant() const {
    return _digits.size() == 1 ||
        (_digits.size() > 1 && _digits.back() != 0);
}

std::ostream & operator<<(std::ostream & os, big_uint x) {
    static const char * digits = "0123456789ABCDEF";
    if (x == 0)
        return os << '0';
    vector<char> result;
    digit rem;
    while (x != 0) {
        x /= big_uint::div(x, 10, rem);
        result.push_back(digits[rem]);
    }
    copy(result.rbegin(), result.rend(), ostream_iterator<char>(os));
    return os;
}

}