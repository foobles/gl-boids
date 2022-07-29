//
// Created by foobles on 7/7/2022.
//

#ifndef SDL_GLEW_TEST_MATRIX_HPP
#define SDL_GLEW_TEST_MATRIX_HPP

#include <array>
#include <cmath>

template<typename T, int W, int H>
class Matrix {
public:
    static constexpr int ELEM_COUNT = W * H;

    constexpr Matrix(std::array<T, ELEM_COUNT> const &arr) noexcept;

    [[nodiscard]] constexpr T const &get(int col, int row) const noexcept;
    [[nodiscard]] constexpr T &get(int col, int row) noexcept;

    [[nodiscard]] constexpr T const &operator[](int i) const noexcept requires (H == 1);
    [[nodiscard]] constexpr T &operator[](int i) noexcept requires (H == 1);

    [[nodiscard]] constexpr T const *data() const noexcept;
    [[nodiscard]] constexpr T *data() noexcept;

    [[nodiscard]] static constexpr Matrix identity() noexcept requires (W == H);

private:
    std::array<T, ELEM_COUNT> arr_;
};

template<typename T>
using Mat4 = Matrix<T, 4, 4>;

template<typename T>
using Vec4 = Matrix<T, 4, 1>;

template<typename T, int W, int H>
constexpr Matrix<T, W, H>::Matrix(std::array<T, ELEM_COUNT> const &arr) noexcept:
    arr_(arr)
{}

template<typename T, int W, int H>
constexpr T const &Matrix<T, W, H>::get(int col, int row) const noexcept {
    return arr_[row*W + col];
}

template<typename T, int W, int H>
constexpr T &Matrix<T, W, H>::get(int col, int row) noexcept {
    return arr_[row*W + col];
}


template<typename T, int W, int H>
constexpr T const &Matrix<T, W, H>::operator[](int i) const noexcept requires (H == 1) {
    return arr_[i];
}

template<typename T, int W, int H>
constexpr T &Matrix<T, W, H>::operator[](int i) noexcept requires (H == 1) {
    return arr_[i];
}


template<typename T, int W, int H>
constexpr T const *Matrix<T, W, H>::data() const noexcept {
    return arr_.data();
}

template<typename T, int W, int H>
constexpr T *Matrix<T, W, H>::data() noexcept {
    return arr_.data();
}

template<typename T, int W, int H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::identity() noexcept requires (W == H) {
    Matrix<T, W, H> ret{{}};
    for (int i = 0; i < W; ++i) {
        ret.get(i, i) = 1;
    }
    return ret;
}

template<typename T, int A, int B, int C>
constexpr Matrix<T, A, B> operator*(Matrix<T, C, B> m1, Matrix<T, A, C> m2) noexcept {
    Matrix<T, A, B> ret{{}};
    for (int x = 0; x < A; ++x) {
        for (int y = 0; y < B; ++y) {
            T acc = 0;
            for (int i = 0; i < C; ++i) {
                acc += m1.get(i, y) * m2.get(x, i);
            }
            ret.get(x, y) = acc;
        }
    }
    return ret;
}



#endif //SDL_GLEW_TEST_MATRIX_HPP
