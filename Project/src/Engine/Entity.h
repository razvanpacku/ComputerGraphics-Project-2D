#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

#include <memory>


class Entity
{
public:
	Entity(std::shared_ptr<Mesh> mesh, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), float rotation = 0.0f, std::shared_ptr<Texture> texture = nullptr);
	~Entity();

	glm::vec2 position;
	float rotation;
	glm::vec2 scale;

	bool useTexture = true;
	void SetTexture(std::shared_ptr<Texture> tex) { texture = tex; }

	glm::mat4 GetModelMatrix() const;
	void Draw() const;
private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Texture> texture = nullptr;
};

