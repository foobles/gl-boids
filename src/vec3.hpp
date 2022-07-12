//
// Created by foobles on 7/10/2022.
//

#ifndef SDL_GLEW_TEST_VEC3_HPP
#define SDL_GLEW_TEST_VEC3_HPP

#include <array>

template<typename T>
class Vec3 {
public:
    constexpr Vec3(T x, T y, T z) noexcept;

    [[nodiscard]] constexpr T const &operator[](int i) const noexcept;
    [[nodiscard]] constexpr T &operator[](int i) noexcept;

    [[nodiscard]] constexpr T const *data() const noexcept;
    [[nodiscard]] constexpr T *data() noexcept;

private:
    std::array<T, 3> arr_;
};

template<typename T>
constexpr Vec3<T>::Vec3(T x, T y, T z) noexcept:
    arr_{x, y, z}
{}

template<typename T>
constexpr T const &Vec3<T>::operator[](int i) const noexcept {
    return arr_[i];
}

template<typename T>
constexpr T &Vec3<T>::operator[](int i) noexcept {
    return arr_[i];
}

template<typename T>
constexpr T const *Vec3<T>::data() const noexcept {
    return arr_.data();
}

template<typename T>
constexpr T *Vec3<T>::data() noexcept {
    return arr_.data();
}


#endif //SDL_GLEW_TEST_VEC3_HPP
