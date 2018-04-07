#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <vector>

struct big_integer {
    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer absolute() const;

    big_integer &operator=(big_integer const &other);

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
    typedef uint32_t digit_t;
    typedef uint64_t double_digit_t;
    static const int DIGIT_BASE = 32;
    static const digit_t DIGIT_MASK = std::numeric_limits<digit_t>().max();

    std::vector<digit_t> digits;
    bool negative;

    big_integer(digit_t a);

    void shrink();

    bool is_zero() const;

    bool is_even() const;

    void negate();

    void bitwise_negate_digits();

    void clear();

    void shift_left_by_words(std::size_t cnt);

    void shift_right_by_words(std::size_t cnt);

    void add_unsigned_shifted_by_words(digit_t a, std::size_t shift = 0);

    void sub_unsigned_shifted_by_words(digit_t a, std::size_t shift = 0);

    void add_unsigned(big_integer other);

    void sub_unsigned(big_integer other);

    void mul_unsigned(digit_t a);

    digit_t div_mod_unsigned(digit_t a);

    big_integer slice_by_words(std::size_t from, std::size_t to);

    void to_complementary2();

    void from_complementary2();
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
