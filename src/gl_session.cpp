//
// Created by foobles on 7/3/2022.
//

#include "gl_session.hpp"

#include <stdexcept>
#include <string>
#include "GL/glew.h"

#include "sdl_session.hpp"
#include "sdl_window.hpp"

GLSession::GLSession(const SdlSession &sdl, const SdlWindow &window, GLConfig config) {
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.major_version) != 0) {
        sdl.throw_current_error("Error setting OpenGL major version");
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.minor_version) != 0) {
        sdl.throw_current_error("Error setting OpenGL minor version");
    }

    gl_context_ = SDL_GL_CreateContext(window.inner());
    if (gl_context_ == nullptr) {
        sdl.throw_current_error("Error creating OpenGL context");
    }


    if (GLenum err = glewInit(); err != GLEW_OK) {
    SDL_GL_DeleteContext(gl_context_);
    auto glew_err_string = reinterpret_cast<char const *>(glewGetErrorString(err));
        throw std::runtime_error(std::string("Error initializing GLEW: ") + glew_err_string);
    }
}

GLSession::~GLSession() noexcept {
    SDL_GL_DeleteContext(gl_context_);
}
