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
#include "matrix.hpp"
#include "transform.hpp"

#include "obj_format.hpp"
#include "mesh.hpp"

char const *VERTEX_SHADER_SOURCE = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 texCoord;

    uniform mat4 uModels[128];
    uniform mat4 uProjection;

    void main() {
        gl_Position = vec4(aPos, 1.0) * uModels[gl_InstanceID] * uProjection;
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

        SDL_Surface *rgb_img = image_loader.load_rgb24_image_flipped("assets/boid.png");
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

        Mesh model = ObjFormat("assets/boid.obj").create_mesh();

        gl.use_program(shader_program);
        glUniform1i(*shader_program.uniform_location("uTex"), 0);

        auto fov_degrees = 80.0f;
        auto fov_radians = fov_degrees * std::numbers::pi_v<GLfloat> / 180.0f;
        auto perspective = Transform<GLfloat>::perspective(fov_radians, window.aspect_ratio(), 0.1, 100.0);
        glUniformMatrix4fv(*shader_program.uniform_location("uProjection"), 1, true, perspective.matrix.data());

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), reinterpret_cast<void const*>(offsetof(Mesh::Vertex, pos)));
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Mesh::Vertex), reinterpret_cast<void const*>(offsetof(Mesh::Vertex, uv)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glEnable(GL_DEPTH_TEST);

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

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GLfloat angle = static_cast<GLfloat>(SDL_GetTicks()) * std::numbers::pi_v<GLfloat> / 5000.0f;
            GLfloat height = std::sin(angle*std::numbers::phi_v<GLfloat>) * 7.0f;
            auto model_transform = Transform<GLfloat>::identity()
                .rotate_y(angle)
                .translate(0, height, -13.0);

            glUniformMatrix4fv(*shader_program.uniform_location("uModels"), 1, true, model_transform.matrix.data());

            gl.use_program(shader_program);
            model.draw_instances(1);

            window.swap_buffers();

            SDL_Delay(1000 / 60);
        }

    } catch(std::runtime_error const &err) {
        SDL_Log(err.what());
        return 1;
    }


    return 0;
}
