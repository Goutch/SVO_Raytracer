//
// Created by User on 31-May-2020.
//

#include "Mesh.h"
#include "glad/glad.h"

Mesh::Mesh() {
    glGenVertexArrays(1, &vao);
}

Mesh::~Mesh() {
    for (auto buffer:buffers) {
        glDeleteBuffers(1, &buffer.second);
    }
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void Mesh::setIndices(const std::vector<unsigned int> &data) {
    bind();
    index_count = data.size();
    if (!has_index_buffer) {
        has_index_buffer = true;
        glGenBuffers(1, &ebo);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned int), data.data(), GL_STATIC_DRAW);
    unbind();
}

void Mesh::setBuffer(unsigned int position, const std::vector<float> &data) {
    setBuffer(position, data.data(), data.size(), sizeof(float), 1);
}

void Mesh::setBuffer(unsigned int position, const std::vector<vec2> &data) {
    setBuffer(position, &data[0].x, data.size() * 2, sizeof(float), 2);
}

void Mesh::setBuffer(unsigned int position, const std::vector<vec3> &data) {
    setBuffer(position, &data[0].x, data.size() * 3, sizeof(float), 3);
}

void Mesh::setBuffer(unsigned int position, const std::vector<vec4> &data) {
    setBuffer(position, &data[0].x, data.size() * 4, sizeof(float), 4);
}

void Mesh::bind() const {
    glBindVertexArray(vao);
}

void Mesh::unbind() const {
    glBindVertexArray(0);
}

void Mesh::setBuffer(unsigned int position,
                     const float *data,
                     unsigned int data_count,
                     const unsigned int byte_size,
                     unsigned int count_per_vertex) {
    bind();
    vertex_count = data_count;
    unsigned int vbo = getVBO(position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data_count * byte_size, data, GL_STATIC_DRAW);
    glVertexAttribPointer(position, count_per_vertex, GL_FLOAT, GL_FALSE, count_per_vertex * byte_size, nullptr);
    glEnableVertexAttribArray(position);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbind();
}

unsigned int Mesh::getVBO(unsigned int position) {
    unsigned int vbo;
    auto vbo_it = buffers.find(position);
    if (vbo_it == buffers.end()) {
        glGenBuffers(1, &vbo);
        buffers.emplace(position, vbo);
    } else {
        vbo = vbo_it->second;
    }
    return vbo;
}

void Mesh::setBuffer(unsigned int position, const std::vector<int> &data) {
    unsigned int vbo = getVBO(position);
    vertex_count = data.size();
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GL_INT), data.data(), GL_STATIC_DRAW);;
    glVertexAttribIPointer(position, 1, GL_INT, sizeof(GL_INT), nullptr);
    glEnableVertexAttribArray(position);
    unbind();
}
void Mesh::setBuffer(unsigned int position, const std::vector<unsigned int> &data) {
    unsigned int vbo = getVBO(position);
    vertex_count = data.size();
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GL_UNSIGNED_INT), data.data(), GL_STATIC_DRAW);;
    glVertexAttribIPointer(position, 1, GL_UNSIGNED_INT, sizeof(GL_UNSIGNED_INT), nullptr);
    glEnableVertexAttribArray(position);
    unbind();
}

bool Mesh::hasIndexBuffer() {
    return has_index_buffer;
}

unsigned int Mesh::getVertexCount() {
    return vertex_count;
}

unsigned int Mesh::getIndexCount() {
    return index_count;
}




