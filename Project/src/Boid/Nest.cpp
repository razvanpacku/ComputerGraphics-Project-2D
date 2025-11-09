#include "./Nest.h"

std::shared_ptr<Texture> Nest::sharedTexture = nullptr;
std::shared_ptr<Mesh> Nest::sharedMesh = nullptr;

Nest::Nest(const glm::vec2& position)
	: Entity(sharedMesh, position, glm::vec2(0.25f), 0.0f, sharedTexture)
{
}

Nest::~Nest()
{
}

void Nest::InitSharedResources(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
{
	sharedMesh = mesh;
	sharedTexture = texture;
}

