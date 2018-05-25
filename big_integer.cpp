#include "big_integer.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// MARK: Implementation details

void big_integer::shrink() {
    while (!digits.empty() && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.empty()) negative = false;
}

bool big_integer::is_zero() const {
    return digits.empty();
}

bool big_integer::is_even() const {
    return is_zero() || (digits.front() & 1) == 0;
}

void big_integer::negate() {
    if (!is_zero()) negative = !negative;
}

void big_integer::clear() {
    digits.clear();
    negative = false;
}

void big_integer::shift_left_by_words(std::size_t cnt) {
    for (std::size_t step = 0; step < cnt; ++step) {
        digits.insert(digits.begin(), 0);
    }
}

void big_integer::shift_right_by_words(std::size_t cnt) {
    if (cnt >= digits.size()) {
        digits.clear();
    } else
        for (size_t step = 0; step < cnt; ++step) {
            digits.erase(digits.begin());
        }

    shrink();
}

void big_integer::add_unsigned_shifted_by_words(digit_vector::digit_t a, std::size_t shift) {
    while (shift >= digits.size()) digits.push_back(0);

    digit_vector::digit_t carry = a;
    for (std::size_t i = shift; i < digits.size() && carry > 0; i++) {
        digit_vector::double_digit_t cur = (digit_vector::double_digit_t) digits[i] + carry;
        if (cur > digit_vector::DIGIT_MASK) {
            carry = (digit_vector::digit_t) ((cur - (cur & digit_vector::DIGIT_MASK)) >> digit_vector::DIGIT_BASE);
        } else {
            carry = 0;
        }
        digits[i] = (digit_vector::digit_t) cur;
    }
    if (carry > 0) digits.push_back(carry);

    shrink();
}

void big_integer::sub_unsigned_shifted_by_words(digit_vector::digit_t a, std::size_t shift) {
    while (shift >= digits.size()) digits.push_back(0);

    digit_vector::digit_t carry = a;
    for (std::size_t i = shift; i < digits.size() && carry > 0; i++) {
        digit_vector::double_digit_t cur = digits[i];
        if (cur < carry) {
            cur = cur + digit_vector::DIGIT_MASK + 1 - carry;
            carry = 1;
        } else {
            cur -= carry;
            carry = 0;
        }
        digits[i] = (digit_vector::digit_t) cur;
    }
    if (carry > 0) throw std::runtime_error("carry is non-zero");

    shrink();
}

void big_integer::add_unsigned(big_integer other) {
    for (std::size_t i = 0; i < other.digits.size(); i++) {
        add_unsigned_shifted_by_words(other.digits[i], i);
    }
}

void big_integer::sub_unsigned(big_integer other) {
    for (std::size_t i = 0; i < other.digits.size(); i++) {
        sub_unsigned_shifted_by_words(other.digits[i], i);
    }
}

void big_integer::mul_unsigned(digit_vector::digit_t a) {
    digit_vector::digit_t carry = 0;
    for (digit_vector::digit_t &digit : digits) {
        digit_vector::double_digit_t cur = (digit_vector::double_digit_t) digit * a + carry;
        digit = (digit_vector::digit_t) (cur & digit_vector::DIGIT_MASK);
        carry = (digit_vector::digit_t) (cur >> digit_vector::DIGIT_BASE);
    }
    if (carry > 0) digits.push_back(carry);
    shrink();
}

digit_vector::digit_t big_integer::div_mod_unsigned(digit_vector::digit_t a) {
    digit_vector::digit_t carry = 0;
    for (std::size_t tmp = 0, i = digits.size() - 1; tmp < digits.size(); tmp++, i--) {
        digit_vector::double_digit_t cur = (((digit_vector::double_digit_t) carry) << digit_vector::DIGIT_BASE) + digits[i];
        carry = (digit_vector::digit_t) (cur % a);
        digits[i] = (digit_vector::digit_t) (cur / a);
    }
    shrink();
    return carry;
}

void big_integer::bitwise_negate_digits() {
    for (digit_vector::digit_t &digit : digits) {
        digit = ~digit;
    }
    add_unsigned_shifted_by_words(1);
}

big_integer big_integer::slice_by_words(std::size_t from, std::size_t to) {
    big_integer res = big_integer();
//    res.digits = digit_vector(digits.begin() + from, digits.begin() + to);
    for (std::size_t i = from; i < to; i++) {
        res.digits.push_back(digits[i]);
    }
    return res;
}

void big_integer::to_complementary2() {
    if (negative) {
        negative = false;
        bitwise_negate_digits();
    }
}

void big_integer::from_complementary2() {
    bool neg = !is_zero() && ((digits.back() >> (digit_vector::DIGIT_BASE - 1)) != 0);
    if (neg) {
        bitwise_negate_digits();
        negative = true;
    }
}

// MARK: Constructors

big_integer::big_integer() : digits(), negative(false) {}

big_integer::big_integer(big_integer const &other) noexcept : digits(other.digits), negative(other.negative) {}

big_integer::big_integer(digit_vector::digit_t a) : negative(false) {
    digits.push_back(a);
    shrink();
}

big_integer::big_integer(int a) {
    digits = digit_vector();
    digit_vector::digit_t aa = (digit_vector::digit_t) a;
    if (a < 0) aa = -aa;
    digits.push_back(aa);
    negative = (a < 0);
    shrink();
}

big_integer::big_integer(std::string const &str) {
    bool neg = false;
    for (std::size_t i = 0; i < str.length(); i++) {
        char c = str[i];

        if (i == 0 && c == '-') {
            neg = true;
        } else if (!isdigit(c)) {
            throw std::invalid_argument("non-digit character found in the string");
        } else {
            mul_unsigned(10);
            add_unsigned_shifted_by_words((digit_vector::digit_t) (c - '0'));
        }
    }
    shrink();
    negative = neg;
}

// MARK: Operations

big_integer::~big_integer() = default;

big_integer big_integer::absolute() const {
    big_integer x = *this;
    if (x.negative) x.negate();
    return x;
}

big_integer &big_integer::operator=(big_integer const &other) noexcept {
    this->digits = other.digits;
    this->negative = other.negative;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    big_integer &lhs = *this;

    if (lhs.negative == rhs.negative) {
        lhs.add_unsigned(rhs);
    } else {
        if (rhs.negative) {
            // a > 0 && b < 0
            lhs = lhs - rhs.absolute();
        } else {
            // a < 0 && b > 0
            lhs = rhs - lhs.absolute();
        }
    }

    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    big_integer &lhs = *this;

    if (lhs.negative == rhs.negative) {
        if (lhs.absolute() >= rhs.absolute())
            lhs.sub_unsigned(rhs);
        else {
            lhs = rhs - lhs;
            lhs.negate();
        }
    } else {
        if (rhs.negative) {
            // a > 0 && b < 0
            lhs.add_unsigned(rhs);
        } else {
            // a < 0 && b > 0; -a + b
            lhs.negate();
            lhs.add_unsigned(rhs);
        }
    }

    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer lhs_copy = *this;
    big_integer &lhs = *this;
//    big_integer rhs = r;
//    lhs.clear();
//    lhs.digits.resize(lhs_copy.digits.size() + rhs.digits.size() + 1);
//    std::fill(lhs.digits.begin(), lhs.digits.end(), 0);
    std::size_t sz = lhs_copy.digits.size() + rhs.digits.size() + 1;
    auto *digits = new digit_vector::digit_t[sz];
    std::fill(digits, digits + sz, 0);

    for (std::size_t i = 0; i < lhs_copy.digits.size(); i++) {
        digit_vector::digit_t carry = 0;
        for (std::size_t j = 0; j < rhs.digits.size() || carry > 0; j++) {
            digit_vector::double_digit_t cur = (digit_vector::double_digit_t) digits[i + j] + carry +
                                               (digit_vector::double_digit_t) lhs_copy.digits[i] * (j < rhs.digits.size() ? rhs.digits[j] : 0);
            digits[i + j] = (digit_vector::digit_t) (cur & digit_vector::DIGIT_MASK);
            carry = (digit_vector::digit_t) (cur >> digit_vector::DIGIT_BASE);
        }
    }

//    lhs.digits.clear();
//    for (const auto &d : digits) lhs.digits.push_back(d);
    lhs.digits = digit_vector(digits, sz);

    if (rhs.negative) lhs.negate();
    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator/=(big_integer const &rh) {
    big_integer &lhs = *this;
    big_integer rhs = rh;

    if (lhs.is_zero()) return lhs = 0;
    if (rhs.is_zero()) throw std::invalid_argument("divisor is zero");
    if (lhs.negative != rhs.negative) return lhs = -(lhs.absolute() / rh.absolute());

    lhs.negative = rhs.negative = false;

    if (rhs.digits.size() == 1) {
        lhs.div_mod_unsigned(rhs.digits[0]);
        return lhs;
    }
    if (lhs < rhs) {
        return lhs = 0;
    }

    // Source: https://members.loria.fr/PZimmermann/mca/mca-0.5.pdf

    auto normalization = (digit_vector::digit_t) (((digit_vector::double_digit_t) digit_vector::DIGIT_MASK + 1) / ((digit_vector::double_digit_t) rhs.digits.back() + 1));
    lhs *= normalization;
    rhs *= normalization;

    digit_vector new_digits(lhs.digits.size() - rhs.digits.size() + 1);

    big_integer remainder = lhs.slice_by_words(lhs.digits.size() - rhs.digits.size() + 1, lhs.digits.size());
    digit_vector::double_digit_t highest_digit = (rhs.is_zero() ? 0 : rhs.digits.back());

    for (std::size_t tmp = 0, dig = new_digits.size() - 1; tmp < new_digits.size(); tmp++, dig--) {
        remainder <<= digit_vector::DIGIT_BASE;
        remainder += lhs.digits[dig];

        digit_vector::double_digit_t rem_highest = (remainder.is_zero() ? 0 : remainder.digits.back());
        if (remainder.digits.size() > rhs.digits.size()) {
            rem_highest <<= digit_vector::DIGIT_BASE;
            rem_highest += remainder.digits[remainder.digits.size() - 2];
        }

        digit_vector::double_digit_t quotient = rem_highest / highest_digit;
        if (quotient >= digit_vector::DIGIT_MASK) quotient = digit_vector::DIGIT_MASK;

        big_integer mul_dq = rhs * (digit_vector::digit_t) (quotient & digit_vector::DIGIT_MASK);
        while (remainder < mul_dq) {
            quotient--;
            mul_dq -= rhs;
        }

        new_digits[dig] = (digit_vector::digit_t) (quotient & digit_vector::DIGIT_MASK);
        remainder -= mul_dq;
    }

    lhs.digits = new_digits;
    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    big_integer q = *this / rhs;
    return *this -= rhs * q;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    big_integer &lhs = *this;

    big_integer new_rhs = rhs;
    lhs.to_complementary2();
    new_rhs.to_complementary2();

    std::size_t size = std::min(lhs.digits.size(), new_rhs.digits.size());
    digit_vector new_digits(size);
#pragma omp parallel for
    for (std::size_t i = 0; i < size; i++) {
        new_digits[i] = (lhs.digits[i] & new_rhs.digits[i]);
    }
    lhs.digits = new_digits;
    lhs.from_complementary2();

    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    big_integer &lhs = *this;

    big_integer new_rhs = rhs;
    lhs.to_complementary2();
    new_rhs.to_complementary2();

    std::size_t size = std::max(lhs.digits.size(), new_rhs.digits.size());
    digit_vector new_digits(size);
#pragma omp parallel for
    for (std::size_t i = 0; i < size; i++) {
        new_digits[i] = ((i < lhs.digits.size() ? lhs.digits[i] : 0) |
                         (i < new_rhs.digits.size() ? new_rhs.digits[i] : 0));
    }
    lhs.digits = new_digits;
    lhs.from_complementary2();

    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    big_integer &lhs = *this;

    big_integer new_rhs = rhs;
    lhs.to_complementary2();
    new_rhs.to_complementary2();

    std::size_t size = std::max(lhs.digits.size(), new_rhs.digits.size());
    digit_vector new_digits(size);
#pragma omp parallel for
    for (std::size_t i = 0; i < size; i++) {
        new_digits[i] = ((i < lhs.digits.size() ? lhs.digits[i] : 0) ^
                         (i < new_rhs.digits.size() ? new_rhs.digits[i] : 0));
    }
    lhs.digits = new_digits;
    lhs.from_complementary2();

    return lhs;
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs < 0) operator>>=(-rhs);

    big_integer &lhs = *this;
    lhs.shift_left_by_words(rhs / digit_vector::DIGIT_MASK);
    auto bound = (rhs % digit_vector::DIGIT_MASK);
    for (digit_vector::digit_t step = 0; step < bound; step++) {
        lhs.mul_unsigned(2);
    }
    lhs.shrink();
    return lhs;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs < 0) operator<<=(-rhs);

    big_integer &lhs = *this;
    lhs.shift_right_by_words(rhs / digit_vector::DIGIT_MASK);
    auto bound = (rhs % digit_vector::DIGIT_MASK);
    for (digit_vector::digit_t step = 0; step < bound; step++) {
        lhs.div_mod_unsigned(2);
    }
    if (lhs.negative) lhs--;
    lhs.shrink();
    return lhs;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer res = *this;
    res.negate();
    return res;
}

big_integer big_integer::operator~() const {
    return -(*this) - 1;
}

big_integer &big_integer::operator++() {
    if (!negative)
        add_unsigned_shifted_by_words(1);
    else
        sub_unsigned_shifted_by_words(1);
    shrink();
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    ++*this;
    return res;
}

big_integer &big_integer::operator--() {
    if (!negative)
        sub_unsigned_shifted_by_words(1);
    else
        add_unsigned_shifted_by_words(1);
    shrink();
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    --*this;
    return res;
}

big_integer operator+(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    big_integer res = a;
    return res ^= b;
}

big_integer operator<<(big_integer a, int bits) {
    big_integer res = a;
    return res <<= bits;
}

big_integer operator>>(big_integer a, int bits) {
    big_integer res = a;
    return res >>= bits;
}

// MARK: Comparisons

bool operator==(big_integer const &a, big_integer const &b) {
    return a.negative == b.negative && a.digits == b.digits;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.negative && !b.negative) return true;
    if (!a.negative && b.negative) return false;

    if (a.digits.size() != b.digits.size())
        return a.negative ?
               a.digits.size() > b.digits.size() :
               a.digits.size() < b.digits.size();

    for (std::size_t j = 0, i = a.digits.size() - 1; j < a.digits.size(); j++, i--) {
        if (a.digits[i] != b.digits[i])
            return a.negative ?
                   a.digits[i] > b.digits[i] :
                   a.digits[i] < b.digits[i];
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return a == b || a < b;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return a == b || a > b;
}

std::string to_string(big_integer const &a) {
    if (a.is_zero()) return "0";

    big_integer x = a;
    bool neg = x.negative;

    std::string res;
    while (!x.is_zero()) {
        auto d = x.div_mod_unsigned(10);
        res.insert(0, std::to_string(d));
    }
    if (neg) {
        res.insert(0, "-");
    }

    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
