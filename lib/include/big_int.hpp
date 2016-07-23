#pragma once

#include <iostream>
#include <deque>
#include <string>

#include "big_uint.hpp"

namespace big {

class big_int {
public:
    enum class sign_t {
        PLUS, MINUS
    };

private:
    sign_t   _sign;
    big_uint _modulus;

    big_int(sign_t s, const big_uint & m);
    big_int(sign_t s, big_uint && m);

public:
    big_int() = default;
    big_int(sdigit x);
    big_int(const std::string & number);
    big_int(const big_int &) = default;
    big_int(big_int &&) = default;
    big_int & operator=(const big_int &) = default;
    big_int & operator=(big_int &&) = default;

    sign_t sign() const;
    
    big_int & operator++();
    big_int & operator--();
    big_int operator++(int);
    big_int operator--(int);

    friend big_int operator+(big_int lhs, sdigit rhs);
    friend big_int operator-(big_int lhs, sdigit rhs);
    friend big_int operator*(big_int lhs, sdigit rhs);
    friend big_int operator/(big_int lhs, sdigit rhs);
    friend big_int operator%(big_int lhs, sdigit rhs);

    friend big_int operator+(sdigit lhs, big_int rhs);
    friend big_int operator-(sdigit lhs, big_int rhs);
    friend big_int operator*(sdigit lhs, big_int rhs);
    friend big_int operator/(sdigit lhs, const big_int & rhs);
    friend big_int operator%(sdigit lhs, const big_int & rhs);

    big_int & negate();

    big_int operator-() const;
    big_int operator+() const;

    big_int & operator+=(sdigit d);
    big_int & operator-=(sdigit d);
    big_int & operator*=(sdigit d);
    big_int & operator/=(sdigit d);
    big_int & operator%=(sdigit d);

    big_int & operator+=(const big_int & x);
    big_int & operator-=(const big_int & x);
    big_int & operator*=(const big_int & x);
    big_int & operator/=(const big_int & x);
    big_int & operator%=(const big_int & x);

    big_int operator+(const big_int & rhs) const;
    big_int operator-(const big_int & rhs) const;
    big_int operator*(const big_int & rhs) const;
    big_int operator/(const big_int & rhs) const;
    big_int operator%(const big_int & rhs) const;

    bool operator==(const big_int & rhs) const;
    bool operator!=(const big_int & rhs) const;
    bool operator<(const big_int & rhs) const;
    bool operator>(const big_int & rhs) const;
    bool operator<=(const big_int & rhs) const;
    bool operator>=(const big_int & rhs) const;

    friend bool operator==(sdigit lhs, const big_int & rhs);
    friend bool operator!=(sdigit lhs, const big_int & rhs);
    friend bool operator<(sdigit lhs, const big_int & rhs);
    friend bool operator>(sdigit lhs, const big_int & rhs);
    friend bool operator<=(sdigit lhs, const big_int & rhs);
    friend bool operator>=(sdigit lhs, const big_int & rhs);
    friend bool operator<(const big_int & lhs, sdigit rhs);
    friend bool operator>(const big_int & lhs, sdigit rhs);
    friend bool operator<=(const big_int & lhs, sdigit rhs);
    friend bool operator>=(const big_int & lhs, sdigit rhs);

    bool satisfies_invariant() const;

    big_int pow(digit e) const;

    friend std::ostream & operator<<(std::ostream & os, const big_int & x);
    friend std::istream & operator>>(std::istream & os, big_int & x);
};

}
