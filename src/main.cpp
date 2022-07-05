#include <stdexcept>

#include "GL/glew.h"
#include "SDL_log.h"
#include "SDL_events.h"
#include "SDL_timer.h"

#include "sdl_session.hpp"
#include "sdl_window.hpp"
#include "gl_session.hpp"
#include "sdl_image_loader.hpp"
#include "gl_shader_program.hpp"

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

        gl.use_program(shader_program);
        glUniform1i(glGetUniformLocation(shader_program.inner(), "uTex"), 0);

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
