#pragma once

#include <vector>

namespace walle::asymtx {

template <typename T>
class basic_matrix_t {
    basic_matrix_t(std::vector<T> buffer, std::size_t rows, std::size_t cols)
        : _buffer(std::move(buffer))
        , _rows(rows)
        , _cols(cols) {}

public:
    static basic_matrix_t make();
    static basic_matrix_t make(std::size_t rows, std::size_t cols, const T& value);

    const T& at(std::size_t i, std::size_t j) const {
        return _buffer[i * _cols + j];
    }

    T& at(std::size_t i, std::size_t j) {
        return _buffer[i * _cols + j];
    }

    const T& checked_at(std::size_t i, std::size_t j) const {
        if (i >= _rows || j > _cols) {
            throw std::invalid_argument {"Out of range"};
        }

        return at(i, j);
    }

    T& checked_at(int i, int j) {
        if (i >= _rows || j > _cols) {
            throw std::invalid_argument {"Out of range"};
        }

        return at(i, j);
    }

    const T* data() const noexcept {
        return _buffer.data();
    }
    T* unsafe_data() noexcept {
        return _buffer.data();
    }
    std::size_t rows() const noexcept {
        return _rows;
    }
    std::size_t cols() const noexcept {
        return _cols;
    }
    bool is_empty() const noexcept {
        return _buffer.empty();
    }

private:
    std::vector<T> _buffer {};
    std::size_t _rows {};
    std::size_t _cols {};
};

template <typename T>
basic_matrix_t<T> basic_matrix_t<T>::make() {
    return basic_matrix_t {{}, 0, 0};
}

template <typename T>
basic_matrix_t<T> basic_matrix_t<T>::make(std::size_t rows, std::size_t cols, const T& value) {
    if (rows == 0 || cols == 0) {
        throw std::invalid_argument {"Invalid input"};
    }
    std::vector<T> buffer(rows * cols, value);
    return basic_matrix_t {std::move(buffer), rows, cols};
}

} // namespace walle::asymtx
