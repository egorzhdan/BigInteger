#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <vector>
#include "digit_vector.h"

struct big_integer {
    big_integer();

    big_integer(big_integer const &other) noexcept;

    big_integer(int a); // NOLINT

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer absolute() const;

    big_integer &operator=(big_integer const &other) noexcept;

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &r);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend std::string to_string(big_integer const &a);

private:
    digit_vector digits;
    bool negative;

    big_integer(digit_vector::digit_t a); // NOLINT

    void shrink();

    bool is_zero() const;

    bool is_even() const;

    void negate();

    void bitwise_negate_digits();

    void clear();

    void shift_left_by_words(std::size_t cnt);

    void shift_right_by_words(std::size_t cnt);

    void add_unsigned_shifted_by_words(digit_vector::digit_t a, std::size_t shift = 0);

    void sub_unsigned_shifted_by_words(digit_vector::digit_t a, std::size_t shift = 0);

    void add_unsigned(big_integer other);

    void sub_unsigned(big_integer other);

    void mul_unsigned(digit_vector::digit_t a);

    digit_vector::digit_t div_mod_unsigned(digit_vector::digit_t a);

    big_integer slice_by_words(std::size_t from, std::size_t to);

    void to_complementary2();

    void from_complementary2();

    template<class Function>
    void apply_bitwise_operation(big_integer const &rhs, Function function);
};

big_integer operator+(big_integer a, big_integer const &b);

big_integer operator-(big_integer a, big_integer const &b);

big_integer operator*(big_integer a, big_integer const &b);

big_integer operator/(big_integer a, big_integer const &b);

big_integer operator%(big_integer a, big_integer const &b);

big_integer operator&(big_integer a, big_integer const &b);

big_integer operator|(big_integer a, big_integer const &b);

big_integer operator^(big_integer a, big_integer const &b);

big_integer operator<<(big_integer a, int bits);

big_integer operator>>(big_integer a, int bits);

std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif // BIG_INTEGER_H
