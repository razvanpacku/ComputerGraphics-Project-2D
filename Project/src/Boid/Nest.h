#pragma once
#include "../Engine/Entity.h"

class Nest : public Entity
{
public:
	Nest(const glm::vec2& position);
	~Nest();

	static void InitSharedResources(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
private:
	static std::shared_ptr<Mesh> sharedMesh;
	static std::shared_ptr<Texture> sharedTexture;
};

