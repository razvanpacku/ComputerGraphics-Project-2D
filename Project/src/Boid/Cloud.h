#pragma once
#include "../Engine/Entity.h"

#define CLOUD_SPEED_FACTOR 2.0f

class Cloud : public Entity
{
public:
	Cloud(const glm::vec2& position, const glm::vec2& scale, float opacity, float height, float leftBound, float rightBound);
	~Cloud();

	static void InitSharedResources(std::shared_ptr<Mesh> mesh, std::vector<std::shared_ptr<Texture>> textures);

	float getHeight() const { return height; }

	void Update(float deltaTime);
private:
	static std::shared_ptr<Mesh> sharedMesh;
	static std::vector<std::shared_ptr<Texture>> sharedTextures;

	float height;
	float leftBound, rightBound;
};

