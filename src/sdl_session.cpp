//
// Created by foobles on 7/3/2022.
//

#include "sdl_session.hpp"

#include <stdexcept>
#include <string>
#include "SDL.h"

SdlSession::SdlSession()  {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        throw std::runtime_error(std::string("Error initializing SDL: ") + SDL_GetError());
    }
}

SdlSession::~SdlSession() noexcept {
    SDL_Quit();
}

void SdlSession::throw_current_error(std::string &&msg) const  {
    throw std::runtime_error(std::move(msg) + ": " + SDL_GetError());
}
