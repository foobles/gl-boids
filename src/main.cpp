#include <numbers>
#include <cmath>
#include <stdexcept>

#include "GL/glew.h"
#include "SDL_log.h"
#include "SDL_events.h"
#include "SDL_timer.h"

#include "sdl_session.hpp"
#include "sdl_window.hpp"
#include "sdl_image_loader.hpp"
#include "gl_session.hpp"
#include "gl_shader_program.hpp"
#include "mat4.hpp"

char const *VERTEX_SHADER_SOURCE = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 texCoord;

    uniform mat4 uModel;
    uniform mat4 uPerspective;

    void main() {
        gl_Position = vec4(aPos, 1.0) * uModel * uPerspective;
        texCoord = aTexCoord;
    }
)";

char const *FRAGMENT_SHADER_SOURCE = R"(
    #version 330 core
    in vec2 texCoord;

    out vec4 fragColor;

    uniform sampler2D uTex;

    void main() {
        fragColor = texture(uTex, texCoord);
    }
)";


int main(int argc, char *argv[]) {
    try {
        SdlSession sdl;
        SdlWindow window(sdl, {
                .width = 640,
                .height = 400,
                .title = "Hello"
        });
        GLSession gl(sdl, window, {
                .major_version = 3,
                .minor_version = 3
        });

        SdlImageLoader image_loader(sdl);

        auto shader_program = GLShaderProgramBuilder()
                .vertex_shader(VERTEX_SHADER_SOURCE)
                .fragment_shader(FRAGMENT_SHADER_SOURCE)
                .build(gl);

        SDL_Surface *rgb_img = image_loader.load_rgb24_image_flipped("assets/wall.jpg");
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
                // positions           // texture coords
                 4.0f, 0.0f, -13.0f,   1.0f, 1.0f,   // top right
                 4.0f, 0.0f, -5.0f,    1.0f, 0.0f,   // bottom right
                -4.0f, 0.0f, -5.0f,    0.0f, 0.0f,   // bottom left
                -4.0f, 0.0f, -13.0f,   0.0f, 1.0f,   // top left
        };

        GLuint indices[] = {
                0, 1, 3,
                1, 2, 3
        };

        GLuint vao;
        glGenVertexArrays(1, &vao);

        GLuint buffers[2];
        glGenBuffers(2, buffers);

        auto [vbo, ebo] = buffers;

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(GLfloat), (void *) (0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        gl.use_program(shader_program);
        glUniform1i(*shader_program.uniform_location("uTex"), 0);

        auto fov_degrees = 80.0f;
        auto fov_radians = fov_degrees * std::numbers::pi_v<GLfloat> / 180.0f;
        auto perspective = Mat4<GLfloat>::perspective(fov_radians, window.aspect_ratio(), 0.1, 100.0);
        glUniformMatrix4fv(*shader_program.uniform_location("uPerspective"), 1, true, perspective.data());

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

            glClear(GL_COLOR_BUFFER_BIT);

            GLfloat height = std::sin(static_cast<GLfloat>(SDL_GetTicks()) * std::numbers::pi_v<GLfloat> / 5000.0f) * 5.0f;
            auto model = Mat4<GLfloat>::identity().translate(0, height, 0);
            glUniformMatrix4fv(*shader_program.uniform_location("uModel"), 1, true, model.data());

            glBindVertexArray(vao);
            gl.use_program(shader_program);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            window.swap_buffers();

            SDL_Delay(1000 / 60);
        }
    } catch(std::runtime_error const &err) {
        SDL_Log(err.what());
        return 1;
    }
    return 0;
}
