//
// Created by foobles on 7/4/2022.
//

#include "gl_shader_program.hpp"

#include <stdexcept>
#include <format>

class GLShaderCompiler {
public:
    explicit GLShaderCompiler(GLSession const &gl):
        program_(glCreateProgram()),
        vertex_shader_(0),
        fragment_shader_(0)
    {}

    ~GLShaderCompiler() noexcept {
        free_all();
    }

    GLShaderCompiler(GLShaderCompiler const &other) = delete;
    GLShaderCompiler(GLShaderCompiler &&other) noexcept:
        program_(other.program_),
        vertex_shader_(other.vertex_shader_),
        fragment_shader_(other.fragment_shader_)
    {
        other.make_zero();
    }

    GLShaderCompiler &operator=(GLShaderCompiler const &other) = delete;
    GLShaderCompiler &operator=(GLShaderCompiler &&other) noexcept {
        free_all();
        program_ = other.program_;
        vertex_shader_ = other.vertex_shader_;
        fragment_shader_ = other.fragment_shader_;
        other.make_zero();
        return *this;
    }

    void compile_vertex_shader(std::string_view src) {
        if (vertex_shader_ != 0) {
            throw std::runtime_error("Cannot compile vertex shader: vertex shader already exists");
        }
        vertex_shader_ = compile_shader(GL_VERTEX_SHADER, "vertex shader", src);
    }

    void compile_fragment_shader(std::string_view src) {
        if (fragment_shader_ != 0) {
            throw std::runtime_error("Cannot compile fragment shader: fragment shader already exists");
        }
        fragment_shader_ = compile_shader(GL_FRAGMENT_SHADER, "fragment shader", src);
    }

    void link() const {
        glLinkProgram(program_);
        if (
            GLint ok;
            glGetProgramiv(program_, GL_LINK_STATUS, &ok),
            !ok
        ) {
            std::string err_string = "Error linking shader program: ";
            auto info_offset = err_string.size();
            GLint info_size;
            glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &info_size);
            err_string.resize(err_string.size() + info_size - 1);
            glGetProgramInfoLog(program_, info_size, nullptr, err_string.data() + info_offset);
            throw std::runtime_error(err_string);
        }
    }

    [[nodiscard]] GLuint into_program() && {
        GLuint ret = program_;
        free_shaders();
        make_zero();
        return ret;
    }

private:
    void make_zero() noexcept {
        program_ = 0;
        vertex_shader_ = 0;
        fragment_shader_ = 0;
    }

    void free_all() const noexcept {
        free_shaders();
        if (program_ != 0) {
            glDeleteProgram(program_);
        }
    }

    void free_shaders() const noexcept {
        if (vertex_shader_ != 0) {
            glDetachShader(program_, vertex_shader_);
            glDeleteShader(vertex_shader_);
        }

        if (fragment_shader_ != 0) {
            glDetachShader(program_, fragment_shader_);
            glDeleteShader(fragment_shader_);
        }
    }

    [[nodiscard]] GLuint compile_shader(GLenum shader_type, char const *shader_name, std::string_view src) const {
        GLuint shader = glCreateShader(shader_type);
        auto src_data = src.data();
        auto src_size = static_cast<GLint>(src.size());
        glShaderSource(shader, 1, &src_data, &src_size);
        glCompileShader(shader);
        if (
            GLint ok;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &ok),
            !ok
        ) {
            std::string err_string = std::format("Error compiling {} shader: ", shader_name);
            auto info_offset = err_string.size();
            GLint info_size;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_size);
            err_string.resize(err_string.size() + info_size - 1);
            glGetShaderInfoLog(shader, info_size, nullptr, err_string.data() + info_offset);
            glDeleteShader(shader);
            puts("deleting uncompiled shader");
            throw std::runtime_error(err_string);
        }
        glAttachShader(program_, shader);
        return shader;
    }

    GLuint program_;
    GLuint vertex_shader_;
    GLuint fragment_shader_;
};

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

GLShaderProgram::GLShaderProgram(GLSession const &gl, const GLShaderProgramBuilder &builder) {
    GLShaderCompiler compiler(gl);
    if (auto src = builder.get_vertex_shader(); !src.empty()) {
        compiler.compile_vertex_shader(src);
    }

    if (auto src = builder.get_fragment_shader(); !src.empty()) {
        compiler.compile_fragment_shader(src);
    }

    compiler.link();
    program_ = std::move(compiler).into_program();
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
