//
// Created by foobles on 7/4/2022.
//

#include "gl_shader_program.hpp"

#include <stdexcept>
#include <format>

GLShaderProgramBuilder &GLShaderProgramBuilder::vertex_shader(std::string_view src) {
    vertex_shader_src_ = src;
    return *this;
}

GLShaderProgramBuilder &GLShaderProgramBuilder::fragment_shader(std::string_view src) {
    fragment_shader_src_ = src;
    return *this;
}

GLShaderProgram GLShaderProgramBuilder::build(GLSession const &gl) const {
    return {gl, *this};
}

std::string_view GLShaderProgramBuilder::get_vertex_shader() const {
    return vertex_shader_src_;
}

std::string_view GLShaderProgramBuilder::get_fragment_shader() const {
    return fragment_shader_src_;
}

GLuint GLShaderProgram::inner() const {
    return program_;
}

class AttachedGLShader {
public:
    AttachedGLShader() noexcept:
        program_(),
        shader_(0)
    {}

    AttachedGLShader(GLuint program, GLenum type, char const *shader_name, std::string_view src):
        program_(program),
        shader_(glCreateShader(type))
    {
        auto src_len = static_cast<GLint>(src.length());
        auto src_ptr = static_cast<GLchar const *>(src.data());
        glShaderSource(shader_, 1, &src_ptr, &src_len);
        glCompileShader(shader_);
        if (
            GLint ok;
            glGetShaderiv(shader_, GL_COMPILE_STATUS, &ok),
            !ok
        ) {
            std::string error_string = std::format("Error compiling {}: ", shader_name);
            auto info_offset = error_string.size();
            GLint info_len;
            glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &info_len);
            error_string.resize(error_string.size() + info_len - 1);
            glGetShaderInfoLog(shader_, info_len, nullptr, error_string.data() + info_offset);
            glDeleteShader(shader_);
            throw std::runtime_error(error_string);
        }
        glAttachShader(program_, shader_);
    }

    ~AttachedGLShader() noexcept {
        delete_current_shader();
    }

    AttachedGLShader(AttachedGLShader &&other) noexcept:
        program_(other.program_),
        shader_(other.shader_)
    {
        other.shader_ = 0;
    }

    AttachedGLShader &operator=(AttachedGLShader &&other) noexcept {
        delete_current_shader();
        program_ = other.program_;
        shader_ = other.shader_;
        other.shader_ = 0;
        return *this;
    }

private:
    void delete_current_shader() const noexcept {
        if (shader_ != 0) {
            glDetachShader(program_, shader_);
            glDeleteShader(shader_);
        }
    }

    GLuint program_;
    GLuint shader_;
};

GLShaderProgram::GLShaderProgram(GLSession const &gl, const GLShaderProgramBuilder &builder):
    program_(glCreateProgram())
{
    try {
        AttachedGLShader vertex_shader;
        AttachedGLShader fragment_shader;

        if (auto src = builder.get_vertex_shader(); !src.empty()) {
            vertex_shader = AttachedGLShader(program_, GL_VERTEX_SHADER, "vertex shader", src);
        }

        if (auto src = builder.get_fragment_shader(); !src.empty()) {
            fragment_shader = AttachedGLShader(program_, GL_FRAGMENT_SHADER, "fragment shader", src);
        }

        glLinkProgram(program_);
        if (
            GLint ok;
            glGetProgramiv(program_, GL_LINK_STATUS, &ok),
            !ok
        ) {
            std::string error_string = "Error linking program: ";
            auto info_offset = error_string.size();
            GLint info_len;
            glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &info_len);
            error_string.resize(error_string.size() + info_len - 1);
            glGetProgramInfoLog(program_, info_len, nullptr, error_string.data() + info_offset);
            throw std::runtime_error(error_string);
        }
    } catch (...) {
        glDeleteProgram(program_);
        throw;
    }
}

GLShaderProgram::~GLShaderProgram() noexcept {
    delete_current_program();
}

GLShaderProgram::GLShaderProgram(GLShaderProgram &&other) noexcept:
    program_(other.program_)
{
    other.program_ = 0;
}

GLShaderProgram &GLShaderProgram::operator=(GLShaderProgram &&other) noexcept {
    delete_current_program();
    program_ = std::move(other).into_inner();
    return *this;
}

GLuint GLShaderProgram::into_inner() && {
    GLuint ret = program_;
    program_ = 0;
    return ret;
}

void GLShaderProgram::delete_current_program() const noexcept {
    if (program_ != 0) {
        glDeleteProgram(program_);
    }
}
