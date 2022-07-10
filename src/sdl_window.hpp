//
// Created by foobles on 7/3/2022.
//

#ifndef SDL_GLEW_TEST_SDL_WINDOW_HPP
#define SDL_GLEW_TEST_SDL_WINDOW_HPP

struct SDL_Window;
struct SDL_Renderer;
class GLSession;
class SdlSession;

struct WindowConfig {
    int width;
    int height;
    char const *title;
};

class SdlWindow {
public:
    SdlWindow(SdlSession const &sdl, WindowConfig config);

    ~SdlWindow() noexcept;

    SdlWindow(SdlWindow const &other) = delete;
    SdlWindow(SdlWindow &&other) = delete;
    SdlWindow &operator=(SdlWindow const &other) = delete;
    SdlWindow &operator=(SdlWindow &&other) = delete;

    [[nodiscard]] SDL_Window *inner() const noexcept;

    [[nodiscard]] float aspect_ratio() const noexcept;

    void swap_buffers() const;
private:
    SDL_Window *window_;
};

#endif //SDL_GLEW_TEST_SDL_WINDOW_HPP
