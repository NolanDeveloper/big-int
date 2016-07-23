#include "big_int.hpp"
#include <cmath>
#include <sstream>
#include <tuple>

namespace big {

using namespace std;

big_int::big_int(sign_t s, const big_uint & m) 
        : _sign(s)
        , _modulus(m) { }

big_int::big_int(sign_t s, big_uint && m)
        : _sign(s)
        , _modulus(move(m)) { }

big_int::big_int(sdigit x)
        : _sign(x < 0 ? sign_t::MINUS : sign_t::PLUS)
        , _modulus(abs(x)) { }

// FIXME: This code has many memory allocation. First when _modulus is default
// constructed. Second when isstream is constructed. Third when we read modulus
// from string. The number of acllocations can be reduced to one.
big_int::big_int(const std::string & number)
        : _sign(sign_t::PLUS) {
    istringstream iss(number);
    char c = iss.peek();
    if (c == '+') { iss.ignore(); }
    else if (c == '-') {
        _sign = sign_t::MINUS;
        iss.ignore();
    }
    iss >> _modulus;
}

big_int::sign_t big_int::sign() const {
    return _sign;
}

big_int & big_int::operator++() {
    if (_sign == sign_t::PLUS) 
        ++_modulus;
    else                       
        --_modulus;
    return *this;
}

big_int & big_int::operator--() {
    if (_sign == sign_t::PLUS) 
        --_modulus;
    else                       
        ++_modulus;
    return *this;
}

big_int big_int::operator++(int) {
    big_int prev{ *this };
    ++*this;
    return prev;
}

big_int big_int::operator--(int) {
    big_int prev{ *this };
    --*this;
    return prev;
}

big_int operator+(big_int lhs, sdigit rhs) {
    return lhs += rhs;
}

big_int operator-(big_int lhs, sdigit rhs) {
    return lhs -= rhs;
}

big_int operator*(big_int lhs, sdigit rhs) {
    return lhs *= rhs;
}

big_int operator/(big_int lhs, sdigit rhs) {
    return lhs /= rhs;
}

big_int operator%(big_int lhs, sdigit rhs) {
    return lhs %= rhs;
}

big_int operator+(sdigit lhs, big_int rhs) {
    return rhs += lhs;
}

big_int operator-(sdigit lhs, big_int rhs) {
    return (rhs -= lhs).negate();
}

big_int operator*(sdigit lhs, big_int rhs) {
    return rhs *= lhs;
}

big_int operator/(sdigit lhs, const big_int & rhs) {
    big_int res{ lhs };
    return res /= rhs;
}

big_int operator%(sdigit lhs, const big_int & rhs) {
    big_int res{ lhs };
    return res %= rhs;
}

big_int & big_int::negate() {
    if (_sign == sign_t::PLUS)
        _sign = sign_t::MINUS;
    else
        _sign = sign_t::PLUS;
    return *this;
}

big_int big_int::operator-() const {
    big_int res{ *this };
    return res.negate();
}

big_int big_int::operator+() const {
    return *this;
}

pair<big_int::sign_t, digit> sign_abs(sdigit x) {
    return { x < 0 ? big_int::sign_t::MINUS : big_int::sign_t::PLUS,
             abs(x) };
}

big_int & big_int::operator+=(sdigit d) {
    sign_t s;
    digit a;
    tie(s, a) = sign_abs(d);
    if (s == _sign) {
        _modulus += a;
    } else {
        if (a > _modulus) {
            _modulus = a - _modulus;
            _sign = s;
        } else {
            _modulus -= a;
        }
    }
    return *this;
}

big_int & big_int::operator-=(sdigit d) {
    return *this += -d;
}

big_int & big_int::operator*=(sdigit d) {
    sign_t s;
    digit a;
    tie(s, a) = sign_abs(d);
    _modulus *= a;
    if (s == _sign) {
        _sign = sign_t::PLUS;
    } else {
        _sign = sign_t::MINUS;
    }
    return *this;
}

big_int & big_int::operator/=(sdigit d) {
    sign_t s;
    digit a;
    tie(s, a) = sign_abs(d);
    _modulus /= a;
    if (s == _sign) {
        _sign = sign_t::PLUS;
    } else {
        _sign = sign_t::MINUS;
    }
    return *this;
}

big_int & big_int::operator%=(sdigit d) {
    sign_t s;
    digit a;
    tie(s, a) = sign_abs(d);
    _modulus %= a;
    return *this;
}

big_int big_int::operator+(const big_int & rhs) const {
    if (_sign == rhs._sign)
        return { _sign, _modulus + rhs._modulus };
    else if (_modulus > rhs._modulus)
        return { _sign, _modulus - rhs._modulus };
    return { rhs._sign, rhs._modulus - _modulus };
}

big_int big_int::operator-(const big_int & rhs) const {
    if (_sign != rhs._sign)
        return { _sign, _modulus + rhs._modulus };
    else if (_modulus > rhs._modulus)
        return { _sign, _modulus - rhs._modulus };
    return { (_sign == sign_t::PLUS ? sign_t::MINUS : sign_t::PLUS),
             rhs._modulus - _modulus };
}

big_int big_int::operator*(const big_int & rhs) const {
    if (_sign == rhs._sign)
        return { sign_t::PLUS, _modulus * rhs._modulus };
    return { sign_t::MINUS, _modulus * rhs._modulus };
}

big_int big_int::operator/(const big_int & rhs) const {
    if (_sign == rhs._sign)
        return { sign_t::PLUS, _modulus / rhs._modulus };
    return { sign_t::MINUS, _modulus / rhs._modulus };
}

big_int big_int::operator%(const big_int & rhs) const {
    return { _sign, _modulus % rhs._modulus };
}

big_int & big_int::operator+=(const big_int & x) {
    if (_sign == x._sign)
        _modulus += x._modulus;
    else if (_modulus < x._modulus) {
        _sign = x._sign;
        _modulus = x._modulus - _modulus;
    } else
        _modulus -= x._modulus;
    return *this;
}

big_int & big_int::operator-=(const big_int & x) {
    if (_sign != x._sign)
        _modulus += x._modulus;
    else if (_modulus < x._modulus) {
        negate();
        _modulus = x._modulus - _modulus;
    } else
        _modulus -= x._modulus;
    return *this;
}

big_int & big_int::operator*=(const big_int & x) {
    if (_sign == x._sign)
        _sign = sign_t::PLUS;
    else
        _sign = sign_t::MINUS;
    _modulus *= x._modulus;
    return *this;
}

big_int & big_int::operator/=(const big_int & x) {
    if (x == 0) throw logic_error("zero division");
    if (_sign == x._sign)
        _sign = sign_t::PLUS;
    else
        _sign = sign_t::MINUS;
    _modulus /= x._modulus;
    return *this;
}

big_int & big_int::operator%=(const big_int & x) {
    if (x == 0) throw logic_error("zero division");
    _modulus %= x._modulus;
    return *this;
}

bool big_int::operator==(const big_int & rhs) const {
    return (_modulus == 0u && rhs._modulus == 0u) ||
        (_modulus == rhs._modulus && _sign == rhs._sign);
}

bool big_int::operator!=(const big_int & rhs) const {
    return !(*this == rhs);
}

bool big_int::operator<(const big_int & rhs) const {
    if (_sign == sign_t::MINUS) {
        if (rhs._sign == sign_t::PLUS)
            return true;
        return _modulus > rhs._modulus;
    } else if (rhs._sign == sign_t::MINUS)
        return false;
    return _modulus < rhs._modulus;
}

bool big_int::operator>(const big_int & rhs) const {
    return rhs < *this;
}

bool big_int::operator<=(const big_int & rhs) const {
    return *this < rhs || *this == rhs;
}

bool big_int::operator>=(const big_int & rhs) const {
    return rhs <= *this;
}

bool operator==(sdigit lhs, const big_int & rhs) {
    big_int::sign_t s;
    digit a;
    tie(s, a) = sign_abs(lhs);
    return s == rhs._sign && a == rhs._modulus;
}

bool operator!=(sdigit lhs, const big_int & rhs) {
    return !(lhs == rhs);
}

bool operator<(sdigit lhs, const big_int & rhs) {
    big_int::sign_t s;
    digit a;
    tie(s, a) = sign_abs(lhs);
    if (s == big_int::sign_t::MINUS) {
        if (rhs._sign == big_int::sign_t::PLUS)
            return true;
        return a > rhs._modulus;
    } else if (rhs._sign == big_int::sign_t::MINUS)
        return false;
    return a < rhs._modulus;
}

bool operator>(sdigit lhs, const big_int & rhs) {
    big_int::sign_t s;
    digit a;
    tie(s, a) = sign_abs(lhs);
    if (s == big_int::sign_t::PLUS) {
        if (rhs._sign == big_int::sign_t::MINUS)
            return true;
        return a > rhs._modulus;
    } else if (rhs._sign == big_int::sign_t::PLUS)
        return false;
    return a < rhs._modulus;
}

bool operator<=(sdigit lhs, const big_int & rhs) {
    return lhs < rhs || lhs == rhs;
}

bool operator>=(sdigit lhs, const big_int & rhs) {
    return lhs > rhs || lhs == rhs;
}

bool operator<(const big_int & lhs, sdigit rhs) {
    return rhs > lhs;
}

bool operator>(const big_int & lhs, sdigit rhs) {
    return rhs < lhs;
}

bool operator<=(const big_int & lhs, sdigit rhs) {
    return rhs >= lhs;
}

bool operator>=(const big_int & lhs, sdigit rhs) {
    return rhs <= lhs;
}

bool big_int::satisfies_invariant() const {
    return _modulus.satisfies_invariant() &&
        (_modulus != 0u || _sign == sign_t::PLUS);
}

ostream & operator<<(ostream & os, const big_int & x) {
    if (x._sign == big_int::sign_t::MINUS) 
        os << '-';
    return os << x._modulus;
}

istream & operator>>(istream & os, big_int & x) {
    char c = os.get();
    if (c == '-') {
        x._sign = big_int::sign_t::MINUS;
    } else if (c == '+') {
        x._sign = big_int::sign_t::PLUS;
    } else {
        x._sign = big_int::sign_t::PLUS;
        os.putback(c);
    }
    return os >> x._modulus;
}

}
