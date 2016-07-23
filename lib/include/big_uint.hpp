#pragma once

#include <iostream>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <deque>

namespace big {

using digit      = unsigned int;
using long_digit = unsigned long;
using sdigit     = int;

static_assert(sizeof(long_digit) == 2 * sizeof(digit), 
              "long_digit must be twice as long as digit");
static_assert(std::is_unsigned<digit>::value,
              "digit must be unsigned");
static_assert(std::is_unsigned<long_digit>::value,
              "long_digit must be unsigned");
static_assert(sizeof(sdigit) == sizeof(digit), 
              "sdigit must be as long as digit");
static_assert(std::is_signed<sdigit>::value,
              "sdigit must be signed");

/*
 * The class contains positive integer value of arbitrary length. The value is
 * containing as a sequence of digits in base-2^32 system.
 * It doesn't overflow in multiplication and addition. 
 * Substraction greater from smaller number is undefined behaviour.
 */
class big_uint {
    std::deque<digit> _digits;

    void add_with_shift(const big_uint & x, size_t s);

    big_uint(std::deque<digit> d);

public:
    big_uint();
    explicit big_uint(digit d);
    big_uint(std::initializer_list<digit> digits);
    big_uint(const std::string & num);
    big_uint(const big_uint &) = default;
    big_uint(big_uint &&) = default;
    big_uint & operator=(const big_uint & x) = default;
    big_uint & operator=(big_uint &&) = default;

    const std::deque<digit> digits() const;

    big_uint & operator++();
    big_uint & operator--();
    big_uint operator++(int);
    big_uint operator--(int);

    friend big_uint operator+(const big_uint & lhs, digit rhs);
    friend big_uint operator-(const big_uint & lhs, digit rhs);
    friend big_uint operator*(const big_uint & lhs, digit rhs);
    friend big_uint operator/(const big_uint & lhs, digit rhs);
    friend big_uint operator%(const big_uint & lhs, digit rhs);

    friend big_uint operator+(digit lhs, const big_uint & rhs);
    friend big_uint operator-(digit lhs, const big_uint & rhs);
    friend big_uint operator*(digit lhs, const big_uint & rhs);
    friend big_uint operator/(digit lhs, const big_uint & rhs);
    friend big_uint operator%(digit lhs, const big_uint & rhs);

    big_uint & operator+=(digit d);
    big_uint & operator-=(digit d);
    big_uint & operator*=(digit d);
    big_uint & operator/=(digit d);
    big_uint & operator%=(digit d);

    static big_uint div(const big_uint & dividend, digit divisor, digit & reminder);
    static big_uint div(const big_uint & dividend, digit divisor);

    big_uint operator+(const big_uint & x) const;
    big_uint operator-(const big_uint & x) const;
    big_uint operator*(const big_uint & x) const;
    big_uint operator/(const big_uint & x) const;
    big_uint operator%(const big_uint & x) const;

    big_uint & operator+=(const big_uint & x);
    big_uint & operator-=(const big_uint & x);
    big_uint & operator*=(const big_uint & x);
    big_uint & operator/=(const big_uint & x);
    big_uint & operator%=(const big_uint & x);

    static big_uint div(const big_uint & dividend, const big_uint & divisor, 
                        big_uint & reminder);
    static big_uint div(const big_uint & dividend, const big_uint & divisor);

    bool operator==(const big_uint & rhs) const;
    bool operator!=(const big_uint & rhs) const;
    bool operator<(const big_uint & rhs) const;
    bool operator>(const big_uint & rhs) const;
    bool operator<=(const big_uint & rhs) const;
    bool operator>=(const big_uint & rhs) const;

    friend bool operator<(long_digit lhs, const big_uint & rhs);
    friend bool operator>(long_digit lhs, const big_uint & rhs);
    friend bool operator<=(long_digit lhs, const big_uint & rhs);
    friend bool operator>=(long_digit lhs, const big_uint & rhs);
    friend bool operator==(long_digit lhs, const big_uint & rhs);
    friend bool operator!=(long_digit lhs, const big_uint & rhs);
    friend bool operator<(const big_uint & lhs, long_digit rhs);
    friend bool operator>(const big_uint & lhs, long_digit rhs);
    friend bool operator<=(const big_uint & lhs, long_digit rhs);
    friend bool operator>=(const big_uint & lhs, long_digit rhs);
    friend bool operator==(const big_uint & lhs, long_digit rhs);
    friend bool operator!=(const big_uint & lhs, long_digit rhs);

    friend bool operator<(digit lhs, const big_uint & rhs);
    friend bool operator>(digit lhs, const big_uint & rhs);
    friend bool operator<=(digit lhs, const big_uint & rhs);
    friend bool operator>=(digit lhs, const big_uint & rhs);
    friend bool operator==(digit lhs, const big_uint & rhs);
    friend bool operator!=(digit lhs, const big_uint & rhs);
    friend bool operator<(const big_uint & lhs, digit rhs);
    friend bool operator>(const big_uint & lhs, digit rhs);
    friend bool operator<=(const big_uint & lhs, digit rhs);
    friend bool operator>=(const big_uint & lhs, digit rhs);
    friend bool operator==(const big_uint & lhs, digit rhs);
    friend bool operator!=(const big_uint & lhs, digit rhs);

    big_uint pow(digit e) const;
    //big_uint pow_mod(long e, big_uint mod) const;

    bool satisfies_invariant() const;

    friend std::ostream & operator<<(std::ostream & os, big_uint x);
    friend std::istream & operator>>(std::istream & is, big_uint & x);
};

}
