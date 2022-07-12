//
// Created by foobles on 7/10/2022.
//

#ifndef SDL_GLEW_TEST_VEC4_HPP
#define SDL_GLEW_TEST_VEC4_HPP

#include <array>

template<typename T>
class Vec4 {
public:
    constexpr Vec4(std::array<T, 4> const &arr) noexcept;
    constexpr Vec4(T x, T y, T z) noexcept;
    constexpr Vec4(T x, T y, T z, T w) noexcept;

    [[nodiscard]] constexpr T const &operator[](int i) const noexcept;
    [[nodiscard]] constexpr T &operator[](int i) noexcept;

    [[nodiscard]] constexpr T const *data() const noexcept;
    [[nodiscard]] constexpr T *data() noexcept;

private:
    std::array<T, 4> arr_;
};

template<typename T>
constexpr Vec4<T>::Vec4(std::array<T, 4> const &arr) noexcept:
    arr_(arr)
{}

template<typename T>
constexpr Vec4<T>::Vec4(T x, T y, T z) noexcept:
    Vec4{x, y, z, 1}
{}

template<typename T>
constexpr Vec4<T>::Vec4(T x, T y, T z, T w) noexcept:
    arr_{x, y, z, w}
{}


template<typename T>
constexpr T const &Vec4<T>::operator[](int i) const noexcept {
    return arr_[i];
}

template<typename T>
constexpr T &Vec4<T>::operator[](int i) noexcept {
    return arr_[i];
}

template<typename T>
constexpr T const *Vec4<T>::data() const noexcept {
    return arr_.data();
}

template<typename T>
constexpr T *Vec4<T>::data() noexcept {
    return arr_.data();
}


#endif //SDL_GLEW_TEST_VEC4_HPP
