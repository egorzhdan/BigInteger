#ifndef BIGINTEGER_DIGIT_VECTOR_H
#define BIGINTEGER_DIGIT_VECTOR_H

#include <cstdint>
#include <memory>
#include <limits>

struct digit_vector {
public:
    typedef uint32_t digit_t;
    typedef uint64_t double_digit_t;
    static const int DIGIT_BASE = 32;
    static const digit_t DIGIT_MASK = std::numeric_limits<digit_t>::max();

    digit_vector() noexcept;

    explicit digit_vector(std::size_t initial_size);

    digit_vector(digit_t* ptr, std::size_t size);

    digit_vector(const digit_vector& rhs);

    void push_back(const digit_t &item);

    void pop_back();

    std::size_t size() const;

    bool empty() const;

    void clear();

    const digit_t &back() const;

    const digit_t &front() const;

    void resize(std::size_t new_size);

    bool operator==(const digit_vector &rhs) const;

    digit_t &operator[](std::size_t idx);

    const digit_t &operator[](std::size_t idx) const;

    digit_vector &operator=(const digit_vector& rhs);

    ~digit_vector();

    typedef digit_t *iterator;

    typedef const digit_t *const_iterator;

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    void insert(const_iterator pos, const digit_t &value);

    void erase(const_iterator pos);

    template<typename Iterator>
    digit_vector(Iterator first, Iterator last);

    void prepare_mutation();

private:
    struct big_storage {
        std::size_t capacity;
        std::shared_ptr<digit_t> data;

        ~big_storage();
    };

    union {
        digit_t small;
        big_storage big;
    };

    bool is_small;

    std::size_t _size;

    void increase_capacity();

    void decrease_capacity();
};

#endif //BIGINTEGER_DIGIT_VECTOR_H
