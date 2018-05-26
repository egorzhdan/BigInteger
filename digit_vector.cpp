#include "digit_vector.h"

digit_vector::digit_vector() noexcept : small(0), is_small(true), _size(0) {}

digit_vector::digit_vector(std::size_t initial_size) : digit_vector() {
    if (initial_size <= 1) {
        is_small = true;
        small = 0;
        _size = initial_size;
    } else {
        is_small = false;
        _size = big.capacity = initial_size;

        new(&big.data) std::shared_ptr<digit_t>(new digit_t[_size], std::default_delete<digit_t[]>());
    }
}

digit_vector::digit_vector(const digit_vector &rhs) {
    if (rhs.is_small) {
        is_small = true;
        small = rhs.small;
        _size = rhs._size;
    } else {
        is_small = false;
        _size = rhs._size;
        big.capacity = rhs.big.capacity;

        new(&big.data) std::shared_ptr<digit_t>(rhs.big.data);
    }
}

digit_vector::digit_vector(digit_vector::digit_t *ptr, std::size_t size) {
    assert(size > 0);

    _size = size;
    if (size == 1) {
        is_small = true;
        small = ptr[0];
    } else {
        is_small = false;
        big.capacity = size;

        new(&big.data) std::shared_ptr<digit_t>(ptr, std::default_delete<digit_t[]>());
    }
}

digit_vector::big_storage::~big_storage() = default;

digit_vector::~digit_vector() {
    if (!is_small) big.~big_storage();
}

std::size_t digit_vector::size() const {
    return _size;
}

bool digit_vector::empty() const {
    return _size == 0;
}

void digit_vector::clear() {
    is_small = true;
    _size = 0;
    small = 0;
}

void digit_vector::increase_capacity() {
    if (is_small) {
        digit_t backup = small;
        new(&big.data) std::shared_ptr<digit_t>(new digit_t[2]);
        big.data.get()[0] = backup;
        big.capacity = 2;
        is_small = false;
    } else {
        std::size_t new_capacity = 2 * big.capacity;

        auto *clone = new digit_t[new_capacity];
        std::copy(big.data.get(), big.data.get() + big.capacity, clone);

        new(&big.data) std::shared_ptr<digit_t>(clone);
        big.capacity = new_capacity;
    }
}

void digit_vector::decrease_capacity() {
}

void digit_vector::push_back(const digit_vector::digit_t &item) {
    prepare_mutation();

    if (_size == 0) {
        is_small = true;
        small = item;
    } else {
        if (is_small || _size == big.capacity) increase_capacity();
        assert(big.capacity > _size);

        big.data.get()[_size] = item;
    }
    _size++;
}

void digit_vector::pop_back() {
    assert(_size > 0);

    if (_size == 1) {
        small = 0;
        _size--;
    } else {
        assert(!is_small);

        _size--;
        if (_size * 2 <= big.capacity) {
            decrease_capacity();
        }
    }
}

digit_vector::iterator digit_vector::begin() {
    prepare_mutation();

    if (is_small) {
        return &small;
    } else {
        return big.data.get();
    }
}

digit_vector::const_iterator digit_vector::begin() const {
    if (is_small) {
        return &small;
    } else {
        return big.data.get();
    }
}

digit_vector::iterator digit_vector::end() {
    prepare_mutation();

    if (is_small) {
        return (&small) + 1;
    } else {
        return big.data.get() + _size;
    }
}

digit_vector::const_iterator digit_vector::end() const {
    if (is_small) {
        return (&small) + 1;
    } else {
        return big.data.get() + _size;
    }
}

const digit_vector::digit_t &digit_vector::back() const {
    if (is_small) return small;
    else return big.data.get()[_size - 1];
}

const digit_vector::digit_t &digit_vector::front() const {
    if (is_small) return small;
    else return big.data.get()[0];
}

const digit_vector::digit_t &digit_vector::operator[](std::size_t idx) const {
    if (is_small) {
        assert(idx == 0);
        return small;
    } else {
        assert(idx < _size);
        return big.data.get()[idx];
    }
}

digit_vector::digit_t &digit_vector::operator[](std::size_t idx) {
    if (is_small) {
        assert(idx == 0);
        return small;
    } else {
        assert(idx < _size);
        prepare_mutation();
        return big.data.get()[idx];
    }
}

void digit_vector::resize(std::size_t new_size) {
    if (new_size <= _size) return;

//    if (is_small) increase_capacity();
//    assert(!is_small);

    while (_size < new_size) push_back(0);
}

bool digit_vector::operator==(const digit_vector &rhs) const {
    if (size() != rhs.size()) return false;

    for (std::size_t i = 0; i < size(); i++) {
        if (operator[](i) != rhs[i]) return false;
    }
    return true;
}

void digit_vector::prepare_mutation() {
    if (is_small) return;
    if (big.data.unique()) return;

    auto *clone = new digit_t[big.capacity];
    std::copy(big.data.get(), big.data.get() + big.capacity, clone);

    new(&big.data) std::shared_ptr<digit_t>(clone);
}

template<typename Iterator>
digit_vector::digit_vector(Iterator first, Iterator last) {
    for (; first != last; first++) {
        push_back(*first);
    }
}

digit_vector &digit_vector::operator=(const digit_vector &rhs) {
    if (*this == rhs) return *this;

    clear();
    if (rhs.is_small) {
        is_small = true;
        small = rhs.small;
        _size = rhs._size;
    } else {
        is_small = false;
        _size = rhs._size;
        big.capacity = rhs.big.capacity;

        new(&big.data) std::shared_ptr<digit_t>(rhs.big.data);
    }
    return *this;
}

void digit_vector::erase(digit_vector::const_iterator pos) {
    std::size_t idx = pos - begin();
    prepare_mutation();

    if (is_small) {
        assert(idx == 0);
        pop_back();
    } else {
        assert(idx < _size);

        for (auto i = idx; i < _size - 1; i++) {
            big.data.get()[i] = big.data.get()[i + 1];
        }
        _size--;
    }
}

void digit_vector::insert(digit_vector::const_iterator pos, const digit_vector::digit_t &value) {
    std::size_t idx = pos - begin();

    if (_size == 0) {
        assert(is_small);
        assert(idx == 0);
        small = value;
    } else {
        if (is_small || big.capacity == _size) increase_capacity();

        for (auto i = _size; i > idx; i--) {
            big.data.get()[i] = big.data.get()[i - 1];
        }
        _size++;
        big.data.get()[idx] = value;
    }
}

digit_vector::reverse_const_iterator digit_vector::rbegin() const {
    return digit_vector::reverse_const_iterator(end());
}

digit_vector::reverse_const_iterator digit_vector::rend() const {
    return digit_vector::reverse_const_iterator(begin());
}
