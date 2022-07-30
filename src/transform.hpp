//
// Created by foobles on 7/29/2022.
//

#ifndef SDL_GLEW_TEST_TRANSFORM_HPP
#define SDL_GLEW_TEST_TRANSFORM_HPP

#include <cmath>
#include "matrix.hpp"

template<typename T>
class Transform {
public:
    Mat4<T> matrix;

    [[nodiscard]] static constexpr Transform identity() noexcept;
    [[nodiscard]] static constexpr Transform perspective(T fov, T ar, T clip_near, T clip_far) noexcept;

    [[nodiscard]] constexpr Transform translate(T dx, T dy, T dz) noexcept;
    [[nodiscard]] constexpr Transform rotate_x(T angle) noexcept;
    [[nodiscard]] constexpr Transform rotate_y(T angle) noexcept;
    [[nodiscard]] constexpr Transform rotate_z(T angle) noexcept;
    [[nodiscard]] constexpr Transform scale(T sx, T sy, T sz) noexcept;
    [[nodiscard]] constexpr Transform scale_uniform(T s) noexcept;

};

template<typename T>
constexpr Transform<T> Transform<T>::identity() noexcept {
    return { Mat4<T>::identity() };
}


template<typename T>
constexpr Transform<T> Transform<T>::perspective(T fov, T ar, T clip_near, T clip_far) noexcept {
    auto xy_scale = 1/std::tan(fov/2);
    auto n = clip_near;
    auto f = clip_far;
    return {{{
        xy_scale, 0,             0,            0,
        0,        xy_scale * ar, 0,            0,
        0,        0,             (n+f)/(n-f), -1,
        0,        0,             2*n*f/(n-f),  0
    }}};
}

template<typename T>
constexpr Transform<T> Transform<T>::translate(T dx, T dy, T dz) noexcept {
    Transform<T> ret = *this;
    ret.matrix.get(0, 3) += dx;
    ret.matrix.get(1, 3) += dy;
    ret.matrix.get(2, 3) += dz;
    return ret;
}

template<typename T>
constexpr Transform<T> Transform<T>::rotate_x(T angle) noexcept {
    auto sin = std::sin(angle);
    auto cos = std::cos(angle);
    return {matrix * Mat4<T>{{
        1,  0,   0,   0,
        0,  cos, sin, 0,
        0, -sin, cos, 0,
        0,  0,   0,   1,
    }}};
}

template<typename T>
constexpr Transform<T> Transform<T>::rotate_y(T angle) noexcept {
    auto sin = std::sin(angle);
    auto cos = std::cos(angle);
    return {matrix * Mat4<T>{{
         cos, 0, sin, 0,
         0,   1, 0,   0,
        -sin, 0, cos, 0,
         0,   0, 0,   1,
    }}};
}

template<typename T>
constexpr Transform<T> Transform<T>::rotate_z(T angle) noexcept {
    auto sin = std::sin(angle);
    auto cos = std::cos(angle);
    return {matrix * Mat4<T>{{
         cos, sin, 0, 0,
        -sin, cos, 0, 0,
         0,   0,   1, 0,
         0,   0,   0, 1,
    }}};
}

template<typename T>
constexpr Transform<T> Transform<T>::scale(T sx, T sy, T sz) noexcept {
    return {matrix * Mat4<T>{{
        sx, 0,  0,  0,
        0,  sy, 0,  0,
        0,  0,  sz, 0,
        0,  0,  0,  1,
    }}};
}

template<typename T>
constexpr Transform<T> Transform<T>::scale_uniform(T s) noexcept {
    return scale(s, s, s);
}


#endif //SDL_GLEW_TEST_TRANSFORM_HPP
