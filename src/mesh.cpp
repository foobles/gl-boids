//
// Created by foobles on 7/16/2022.
//

#include "mesh.hpp"
#include <cstddef>

Mesh::Mesh(std::span<Vertex> vertex_data, std::span<GLuint> element_data) {
    vertex_count = static_cast<GLint>(element_data.size());

    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(2, buffers);
    auto [vertex_buffer, element_buffer] = buffers;

    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(vertex_data.size_bytes()), vertex_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLint>(element_data.size_bytes()), element_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

Mesh::~Mesh() noexcept {
    glDeleteBuffers(2, buffers);
    glDeleteVertexArrays(1, &vertex_array);
}

void Mesh::draw() const {
    glBindVertexArray(vertex_array);
    glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, nullptr);
}
