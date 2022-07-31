//
// Created by foobles on 7/7/2022.
//

#ifndef SDL_GLEW_TEST_MATRIX_HPP
#define SDL_GLEW_TEST_MATRIX_HPP

#include <array>
#include <cmath>
#include <utility>

template<typename T, int W, int H>
class Matrix {
public:
    static constexpr int ELEM_COUNT = W * H;

    std::array<T, ELEM_COUNT> arr;

    [[nodiscard]] constexpr T const &get(int col, int row) const noexcept;
    [[nodiscard]] constexpr T &get(int col, int row) noexcept;

    [[nodiscard]] constexpr T const &operator[](int i) const noexcept requires (H == 1);
    [[nodiscard]] constexpr T &operator[](int i) noexcept requires (H == 1);

    [[nodiscard]] constexpr T const *data() const noexcept;
    [[nodiscard]] constexpr T *data() noexcept;

    constexpr void transform(auto F) noexcept;

    [[nodiscard]] static constexpr Matrix identity() noexcept requires (W == H);
};


template<typename T>
using Mat4 = Matrix<T, 4, 4>;

template<typename T>
using Vec4 = Matrix<T, 4, 1>;

template<typename T>
using Vec3 = Matrix<T, 3, 1>;


template<typename T, int W, int H>
constexpr T const &Matrix<T, W, H>::get(int col, int row) const noexcept {
    return arr[row * W + col];
}


template<typename T, int W, int H>
constexpr T &Matrix<T, W, H>::get(int col, int row) noexcept {
    return arr[row * W + col];
}


template<typename T, int W, int H>
constexpr T const &Matrix<T, W, H>::operator[](int i) const noexcept requires (H == 1) {
    return arr[i];
}


template<typename T, int W, int H>
constexpr T &Matrix<T, W, H>::operator[](int i) noexcept requires (H == 1) {
    return arr[i];
}


template<typename T, int W, int H>
constexpr T const *Matrix<T, W, H>::data() const noexcept {
    return arr.data();
}

template<typename T, int W, int H>
constexpr T *Matrix<T, W, H>::data() noexcept {
    return arr.data();
}


template<typename T, int W, int H>
constexpr void Matrix<T, W, H>::transform(auto f) noexcept {
    for (int i = 0; i < ELEM_COUNT; ++i) {
        arr[i] = f(i, arr[i]);
    }
}

template<typename T, int W, int H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::identity() noexcept requires (W == H) {
    Matrix<T, W, H> ret{{}};
    for (int i = 0; i < W; ++i) {
        ret.get(i, i) = 1;
    }
    return ret;
}


template<typename T, int A, int B>
constexpr Matrix<T, A, B> &operator+=(Matrix<T, A, B> &m1, Matrix<T, A, B> m2) noexcept {
    m1.transform([&](int i, T cur) { return cur + m2.arr[i]; });
    return m1;
}

template<typename T, int A, int B>
constexpr Matrix<T, A, B> operator+(Matrix<T, A, B> m1, Matrix<T, A, B> m2) noexcept {
    return (m1 += m2);
}

template<typename T, int A, int B>
constexpr Matrix<T, A, B> &operator-=(Matrix<T, A, B> &m1, Matrix<T, A, B> m2) noexcept {
    m1.transform([&](int i, T cur) { return cur - m2.arr[i]; });
    return m1;
}

template<typename T, int A, int B>
constexpr Matrix<T, A, B> operator-(Matrix<T, A, B> m1, Matrix<T, A, B> m2) noexcept {
    return (m1 -= m2);
}

template<typename T, int A, int B>
constexpr Matrix<T, A, B> operator-(Matrix<T, A, B> m) noexcept {
    m.transform([](int i, T cur) { return -cur; });
    return m;
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

template<typename T, int A, int B>
constexpr Matrix<T, A, B> &operator*=(Matrix<T, A, B> &m1, Matrix<T, A, A> m2) noexcept {
    m1 = m1 * m2;
    return m1;
}

template<typename T, int W, int H>
constexpr Matrix<T, W, H> &operator*=(Matrix<T, W, H> &m, T s) noexcept {
    m.transform([=](int, T cur) { return s * cur; });
    return m;
}

template<typename T, int W, int H>
constexpr Matrix<T, W, H> operator*(Matrix<T, W, H> m, T s) noexcept {
    return (m *= s);
}

template<typename T, int W, int H>
constexpr Matrix<T, W, H> operator*(T s, Matrix<T, W, H> m) noexcept {
    return m * s;
}




#endif //SDL_GLEW_TEST_MATRIX_HPP
