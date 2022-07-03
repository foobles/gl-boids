//
// Created by foobles on 7/3/2022.
//

#include "sdl_window.hpp"

#include "SDL_video.h"

#include "sdl_session.hpp"

SdlWindow::SdlWindow(const SdlSession &sdl, WindowConfig config):
window_(SDL_CreateWindow(
        config.title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        config.width,
        config.height,
        SDL_WINDOW_OPENGL))
{
    if (window_ == nullptr) {
        sdl.throw_current_error("Error creating window");
    }
}

SdlWindow::~SdlWindow() noexcept  {
    SDL_DestroyWindow(window_);
}

void SdlWindow::swap_buffers() const  {
    SDL_GL_SwapWindow(window_);
}

SDL_Window *SdlWindow::inner() const noexcept {
    return window_;
}
