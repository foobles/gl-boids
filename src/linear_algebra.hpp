//
// Created by foobles on 7/7/2022.
//

#ifndef SDL_GLEW_TEST_LINEAR_ALGEBRA_HPP
#define SDL_GLEW_TEST_LINEAR_ALGEBRA_HPP

#include <array>
#include <cmath>

template<typename T>
class Mat4 {
public:
    constexpr Mat4(std::array<T, 16> arr) noexcept;

    constexpr T const &get(int col, int row) const noexcept;
    constexpr T &get(int col, int row) noexcept;

    constexpr T const *data() const noexcept;
    constexpr T *data() noexcept;

    constexpr Mat4 translate(T dx, T dy, T dz) const noexcept;

    constexpr static Mat4 perspective(T fov, T ar, T clip_near, T clip_far) noexcept;
    constexpr static Mat4 identity() noexcept;
private:
    std::array<T, 16> arr_;
};

template<typename T>
constexpr Mat4<T>::Mat4(std::array<T, 16> arr) noexcept:
    arr_(arr)
{}

template<typename T>
constexpr T const &Mat4<T>::get(int col, int row) const noexcept {
    return arr_[row*4 + col];
}

template<typename T>
constexpr T &Mat4<T>::get(int col, int row) noexcept {
    return arr_[row*4 + col];
}

template<typename T>
constexpr T const *Mat4<T>::data() const noexcept {
    return arr_.data();
}

template<typename T>
constexpr T *Mat4<T>::data() noexcept {
    return arr_.data();
}

template<typename T>
constexpr Mat4<T> Mat4<T>::translate(T dx, T dy, T dz) const noexcept {
    Mat4 ret = *this;
    ret.get(0, 3) += dx;
    ret.get(1, 3) += dy;
    ret.get(2, 3) += dz;
    return ret;
}

template<typename T>
constexpr Mat4<T> operator*(Mat4<T> a, Mat4<T> b) noexcept {
    Mat4<T> ret{{}};
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            T acc = 0;
            for (int i = 0; i < 4; ++i) {
                acc += a.get(i, y) * b.get(x, i);
            }
            ret.get(x, y) = acc;
        }
    }
    return ret;
}

template<typename T>
constexpr Mat4<T> Mat4<T>::identity() noexcept {
    return {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
}

template<typename T>
constexpr Mat4<T> Mat4<T>::perspective(T fov, T ar, T clip_near, T clip_far) noexcept {
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
