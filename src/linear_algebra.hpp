//
// Created by foobles on 7/7/2022.
//

#ifndef SDL_GLEW_TEST_LINEAR_ALGEBRA_HPP
#define SDL_GLEW_TEST_LINEAR_ALGEBRA_HPP

#include <array>
#include <cmath>

template<typename T, int N, int M>
class Matrix {
public:
    constexpr Matrix(std::array<T, N*M> arr) noexcept;

    constexpr T const &get(int col, int row) const noexcept;
    constexpr T &get(int col, int row) noexcept;

    constexpr T const *data() const noexcept;
    constexpr T *data() noexcept;
private:
    std::array<T, N*M> arr_;
};

template<typename T, int N, int M>
constexpr Matrix<T, N, M>::Matrix(std::array<T, N*M> arr) noexcept:
    arr_(arr)
{}

template<typename T, int N, int M>
constexpr T const &Matrix<T, N, M>::get(int col, int row) const noexcept {
    return arr_[row*N + col];
}

template<typename T, int N, int M>
constexpr T &Matrix<T, N, M>::get(int col, int row) noexcept {
    return arr_[row*N + col];
}

template<typename T, int N, int M>
constexpr T const *Matrix<T, N, M>::data() const noexcept {
    return arr_.data();
}

template<typename T, int N, int M>
constexpr T *Matrix<T, N, M>::data() noexcept {
    return arr_.data();
}

template<typename T, int N, int M, int P>
constexpr Matrix<T, N, M> operator*(Matrix<T, P, M> a, Matrix<T, N, P> b) noexcept {
    Matrix<T, N, M> ret({});
    for (int x = 0; x < N; ++x) {
        for (int y = 0; y < M; ++y) {
            T acc = 0;
            for (int i = 0; i < P; ++i) {
                acc += a.get(i, y) * b.get(x, i);
            }
            ret.get(x, y) = acc;
        }
    }
    return ret;
}

template<typename T, int N>
using Vec = Matrix<T, N, 1>;

template<typename T>
constexpr Matrix<T, 4, 4> perspective_mat4(T fov, T ar, T clip_near, T clip_far) noexcept {
    auto xy_scale = 1/std::tan(fov/2);
    auto n = clip_near;
    auto f = clip_far;
    return {{
        xy_scale, 0,             0,            0,
        0,        xy_scale * ar, 0,            0,
        0,        0,             (n+f)/(f-n), -1,
        0,        0,             2*n*f/(f-n),  0
    }};
}


#endif //SDL_GLEW_TEST_LINEAR_ALGEBRA_HPP
