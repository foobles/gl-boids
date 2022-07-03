#include <string>
#include <stdexcept>
#include <utility>

#include "GL/glew.h"
#include "SDL.h"
#include "SDL_image.h"

char const *VERTEX_SHADER_SOURCE = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 vertexColor;
    out vec2 texCoord;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        vertexColor = aColor;
        texCoord = aTexCoord;
    }
)";

char const *FRAGMENT_SHADER_SOURCE = R"(
    #version 330 core
    in vec3 vertexColor;
    in vec2 texCoord;

    out vec4 fragColor;

    uniform sampler2D uTex;

    void main() {
        fragColor = texture(uTex, texCoord);
    }
)";

struct WindowConfig {
    int width;
    int height;
    char const *title;
};

struct GLConfig {
    int major_version;
    int minor_version;
};

class SdlContext {
public:
    SdlContext() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
            throw std::runtime_error(std::string("Error initializing SDL: ") + SDL_GetError());
        }
    }

    ~SdlContext() noexcept {
        SDL_Quit();
    }

    SdlContext(SdlContext const &other) = delete;
    SdlContext(SdlContext &&other) = delete;
    SdlContext &operator=(SdlContext const &other) = delete;
    SdlContext &operator=(SdlContext &&other) = delete;

    void throw_current_error(std::string &&msg) const {
        throw std::runtime_error(std::move(msg) + ": " + SDL_GetError());
    }
};

class GLContext;

class SdlWindow {
public:
    SdlWindow(SdlContext const &sdl, WindowConfig config):
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

    ~SdlWindow() noexcept {
        SDL_DestroyWindow(window_);
    }

    SdlWindow(SdlWindow const &other) = delete;
    SdlWindow(SdlWindow &&other) = delete;
    SdlWindow &operator=(SdlWindow const &other) = delete;
    SdlWindow &operator=(SdlWindow &&other) = delete;

    void swap_buffers() const {
        SDL_GL_SwapWindow(window_);
    }

    friend class GLContext;
private:
    SDL_Window *window_;
};


class GLContext {
public:
    GLContext(SdlContext const &sdl, SdlWindow const &window, GLConfig config) {
        if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.major_version) != 0) {
            sdl.throw_current_error("Error setting OpenGL major version");
        }

        if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.minor_version) != 0) {
            sdl.throw_current_error("Error setting OpenGL minor version");
        }

        gl_context_ = SDL_GL_CreateContext(window.window_);
        if (gl_context_ == nullptr) {
            sdl.throw_current_error("Error creating OpenGL context");
        }


        if (GLenum err = glewInit(); err != GLEW_OK) {
            SDL_GL_DeleteContext(gl_context_);
            auto glew_err_string = reinterpret_cast<char const *>(glewGetErrorString(err));
            throw std::runtime_error(std::string("Error initializing GLEW: ") + glew_err_string);
        }
    }

    ~GLContext() noexcept {
        SDL_GL_DeleteContext(gl_context_);
    }

    GLContext(GLContext const &other) = delete;
    GLContext(GLContext &&other) = delete;
    GLContext &operator=(GLContext const &other) = delete;
    GLContext &operator=(GLContext &&other) = delete;

private:
    SDL_GLContext gl_context_;
};


class ImageLoader {
public:
    explicit ImageLoader(SdlContext const &sdl):
        sdl_(&sdl)
    {}

    ~ImageLoader() noexcept {
        IMG_Quit();
    }

    [[nodiscard]] SDL_Surface *load_rgb24_image_flipped(char const* path) {
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

private:
    SdlContext const *sdl_;
};


int main(int argc, char *argv[]) {
    try {
        SdlContext sdl;
        SdlWindow window(sdl, {
                .width = 640,
                .height = 400,
                .title = "Hello"
        });
        GLContext gl(sdl, window, GLConfig{
                .major_version = 3,
                .minor_version = 3
        });

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &VERTEX_SHADER_SOURCE, nullptr);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER_SOURCE, nullptr);
        glCompileShader(fragment_shader);

        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        SDL_Surface *rgb_img = ImageLoader(sdl).load_rgb24_image_flipped("assets/wall.jpg");
        GLuint tex;
        glGenTextures(1, &tex);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgb_img->w, rgb_img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_img->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        SDL_FreeSurface(rgb_img);


        GLfloat vertices[] = {
                // positions          // colors           // texture coords
                 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
                 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
                -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
                -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
        };

        GLuint indices[] = {
                0, 1, 3,
                1, 2, 3
        };

        GLuint vao;
        glGenVertexArrays(1, &vao);

        GLuint buffers[2];
        glGenBuffers(2, buffers);

        GLuint vbo = buffers[0];
        GLuint ebo = buffers[1];

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (void *) (0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(GLfloat), (void *) (6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        glUseProgram(shader_program);
        glUniform1i(glGetUniformLocation(shader_program, "uTex"), 0);

        bool running = true;
        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT: {
                        running = false;
                        break;
                    }
                }
            }

            glBindVertexArray(vao);
            glUseProgram(shader_program);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            window.swap_buffers();

            SDL_Delay(1000 / 60);
        }
    } catch(std::runtime_error const &err) {
        SDL_Log(err.what());
    }
    return 0;
}
