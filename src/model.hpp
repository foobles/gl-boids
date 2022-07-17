//
// Created by foobles on 7/16/2022.
//

#ifndef SDL_GLEW_TEST_MODEL_HPP
#define SDL_GLEW_TEST_MODEL_HPP

#include <span>

#include "GL/glew.h"

class Model {
public:
    struct Vertex {
        GLfloat pos[3];
        GLfloat uv[2];
    };

    Model(std::span<Vertex> vertex_data, std::span<GLuint> element_data);
    ~Model() noexcept;

    void draw() const;

private:
    GLuint vertex_array;
    GLuint buffers[2];

    GLint vertex_count;
};

#endif //SDL_GLEW_TEST_MODEL_HPP
