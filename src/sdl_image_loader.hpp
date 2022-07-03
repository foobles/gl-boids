//
// Created by foobles on 7/3/2022.
//

#ifndef SDL_GLEW_TEST_SDL_IMAGE_LOADER_HPP
#define SDL_GLEW_TEST_SDL_IMAGE_LOADER_HPP

struct SDL_Surface;
class SdlSession;

class SdlImageLoader {
public:

    explicit SdlImageLoader(SdlSession const &sdl);

    ~SdlImageLoader() noexcept;

    [[nodiscard]] SDL_Surface *load_rgb24_image_flipped(char const* path) const;

private:
    SdlSession const *sdl_;
};

#endif //SDL_GLEW_TEST_SDL_IMAGE_LOADER_HPP
