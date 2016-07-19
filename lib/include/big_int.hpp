#pragma once

#include <iostream>
#include <deque>
#include <string>

#include "big_uint.hpp"

namespace big {

class big_int {
    enum class sign_t {
        PLUS, MINUS
    };

    static const size_t LENGTH_OF_DIGIT_IN_DEC;

    sign_t   _sign;
    big_uint _digits;

    static big_int school_mul(const big_int & lhs, const big_int & rhs);

public:
    big_int();
    big_int(const std::string & number);
    big_int(const big_int &) = default;
    big_int(big_int &&) = default;
    big_int & operator=(const big_int &) = default;
    big_int & operator=(big_int &&) = default;

    big_int & operator++();
    big_int & operator--();
    big_int operator++(int);
    big_int operator--(int);

    big_int & operator+=(digit d);
    big_int & operator-=(digit d);
    big_int & operator*=(digit d);
    big_int & operator/=(digit d);
    big_int & operator%=(digit d);
    
    big_int & operator+=(const big_int & x);
    big_int & operator-=(const big_int & x);
    big_int & operator*=(const big_int & x);
    //big_int & operator/=(const big_int & x);
    //big_int & operator%=(const big_int & x);

    //big_int operator-() const;
    //big_int operator+() const;

    // ---------------- TEST FRONTIER -----------------

    //big_int operator+(const big_int & rhs) const;
    //big_int operator-(const big_int & rhs) const;
    //big_int operator*(const big_int & rhs) const;
    //big_int operator/(const big_int & rhs) const;
    //big_int operator%(const big_ing & rhs) const;
    
    //bool operator==(const big_int & rhs) const;
    //bool operator!=(const big_int & rhs) const;
    //bool operator<(const big_int & rhs) const;
    //bool operator>(const big_int & rhs) const;
    //bool operator<=(const big_int & rhs) const;
    //bool operator>=(const big_int & rhs) const;
    
    //friend bool operator<(double lhs, const big_int & rhs);
    //friend bool operator>(double lhs, const big_int & rhs);
    //friend bool operator<=(double lhs, const big_int & rhs);
    //friend bool operator>=(double lhs, const big_int & rhs);
    //friend bool operator<(const big_int & lhs, double rhs);
    //friend bool operator>(const big_int & lhs, double rhs);
    //friend bool operator<=(const big_int & lhs, double rhs);
    //friend bool operator>=(const big_int & lhs, double rhs);

    //big_int abs() const;
    //big_int pow(long e) const;
    //big_int pow_mod(long e, big_int mod) const;

    friend std::ostream & operator<<(std::ostream & os, big_int x);
};

}
