//
// Created by foobles on 7/3/2022.
//

#ifndef SDL_GLEW_TEST_SDL_SESSION_HPP
#define SDL_GLEW_TEST_SDL_SESSION_HPP

#include <string>

class SdlSession {
public:
    SdlSession();

    ~SdlSession() noexcept;

    SdlSession(SdlSession const &other) = delete;
    SdlSession(SdlSession &&other) = delete;
    SdlSession &operator=(SdlSession const &other) = delete;
    SdlSession &operator=(SdlSession &&other) = delete;

    void throw_current_error(std::string &&msg) const;
};

#endif //SDL_GLEW_TEST_SDL_SESSION_HPP
