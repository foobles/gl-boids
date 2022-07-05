//
// Created by foobles on 7/4/2022.
//

#ifndef SDL_GLEW_TEST_GL_SHADER_PROGRAM_HPP
#define SDL_GLEW_TEST_GL_SHADER_PROGRAM_HPP

#include <array>
#include <string_view>
#include "GL/glew.h"


class GLSession;
class GLShaderProgramBuilder;

class GLShaderProgram {
public:
    GLShaderProgram(GLSession const &gl, GLShaderProgramBuilder const &builder);
    ~GLShaderProgram() noexcept;

    GLShaderProgram(GLShaderProgram const &other) = delete;
    GLShaderProgram(GLShaderProgram &&other) noexcept;
    GLShaderProgram &operator=(GLShaderProgram const &other) = delete;
    GLShaderProgram &operator=(GLShaderProgram &&other) noexcept;


    [[nodiscard]] GLuint inner() const;
    [[nodiscard]] GLuint into_inner() &&;

private:
    void delete_current_program() const noexcept;

    GLuint program_;
};

class GLShaderProgramBuilder {
public:
    GLShaderProgramBuilder() = default;
    GLShaderProgramBuilder &vertex_shader(std::string_view src);
    GLShaderProgramBuilder &fragment_shader(std::string_view src);

    [[nodiscard]] GLShaderProgram build(GLSession const &gl) const;

    [[nodiscard]] std::string_view get_vertex_shader() const;
    [[nodiscard]] std::string_view get_fragment_shader() const;

private:
    std::string_view vertex_shader_src_;
    std::string_view fragment_shader_src_;
};


#endif //SDL_GLEW_TEST_GL_SHADER_PROGRAM_HPP
