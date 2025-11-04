#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <cstdint>

class Mesh {
public:
    Mesh(
        const std::vector<glm::vec2>& vertices,
        const std::vector<glm::vec3>& colors,
        const std::vector<glm::vec2>& texCoords,
        const std::vector<std::uint32_t>& indices
    );
    ~Mesh();

    void Draw() const;
private:
	GLuint VAO, VBO[3], EBO;
	GLsizei indexCount;
};