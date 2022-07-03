//
// Created by foobles on 7/3/2022.
//

#include "sdl_image_loader.hpp"

#include "SDL_image.h"
#include <stdexcept>
#include <string>

#include "sdl_session.hpp"

SdlImageLoader::SdlImageLoader(const SdlSession &sdl):
        sdl_(&sdl)
{}

SdlImageLoader::~SdlImageLoader() noexcept {
    IMG_Quit();
}

SDL_Surface *SdlImageLoader::load_rgb24_image_flipped(const char *path) const  {
    SDL_Surface *texture_surface = IMG_Load(path);
    if (texture_surface == nullptr) {
        throw std::runtime_error(std::string("Error loading image: ") + IMG_GetError());
    }

    int height = texture_surface->h;
    int width = texture_surface->w;

    SDL_Surface *ret = SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(SDL_PIXELFORMAT_RGB24), SDL_PIXELFORMAT_RGB24);
    if (ret == nullptr) {
        SDL_FreeSurface(texture_surface);
        sdl_->throw_current_error("Error creating RGB24 surface when loading flipped image");
    }

    SDL_Rect src_rect = {0, 0, width, 1};
    SDL_Rect dest_rect = {0, height - 1, width, 1};

    for (int i = 0; i < height; ++i) {
        SDL_BlitSurface(texture_surface, &src_rect, ret, &dest_rect);
        ++src_rect.y;
        --dest_rect.y;
    }

    SDL_FreeSurface(texture_surface);

    if (SDL_SetSurfaceRLE(ret, 0) != 0) {
        SDL_FreeSurface(ret);
        sdl_->throw_current_error("Error setting surface RLE when loading flipped image");
    }

    return ret;
}
