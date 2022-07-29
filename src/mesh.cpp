//
// Created by foobles on 7/16/2022.
//

#include "mesh.hpp"

Mesh::Mesh(std::span<Vertex> vertex_data, std::span<GLuint> element_data) {
    vertex_count = static_cast<GLint>(element_data.size());

    GLuint buffers[2];
    glGenBuffers(2, buffers);
    array_buffer = buffers[0];
    element_array_buffer = buffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(vertex_data.size_bytes()), vertex_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLint>(element_data.size_bytes()), element_data.data(), GL_STATIC_DRAW);
}

Mesh::~Mesh() noexcept {
    GLuint buffers[2] = {array_buffer, element_array_buffer};
    glDeleteBuffers(2, buffers);
}

void Mesh::draw_instances(GLsizei instances) const {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
    glDrawElementsInstanced(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, nullptr, instances);
}
