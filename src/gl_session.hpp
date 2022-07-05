//
// Created by foobles on 7/3/2022.
//

#ifndef SDL_GLEW_TEST_GL_SESSION_HPP
#define SDL_GLEW_TEST_GL_SESSION_HPP

#include "SDL_video.h"

struct GLConfig {
    int major_version;
    int minor_version;
};

class SdlSession;
class SdlWindow;
class GLShaderProgram;

class GLSession {
public:
    GLSession(SdlSession const &sdl, SdlWindow const &window, GLConfig config);

    ~GLSession() noexcept;

    GLSession(GLSession const &other) = delete;
    GLSession(GLSession &&other) = delete;
    GLSession &operator=(GLSession const &other) = delete;
    GLSession &operator=(GLSession &&other) = delete;

    void use_program(GLShaderProgram const &program) const;

private:
    SDL_GLContext gl_context_;
};


#endif //SDL_GLEW_TEST_GL_SESSION_HPP
