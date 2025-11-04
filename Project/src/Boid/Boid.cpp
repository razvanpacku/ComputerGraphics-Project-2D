#include "./Boid.h"

std::shared_ptr<Texture> Boid::sharedTexture = nullptr;
std::shared_ptr<Mesh> Boid::sharedMesh = nullptr;

float VectorToAngle(const glm::vec2& vec)
{
	//return angle in degrees
	return atan2f(vec.y, vec.x) * 180.0f / 3.14159265f - 90.0f;
}

Boid::Boid(const glm::vec2& position, const glm::vec2& velocity)
	: Entity(sharedMesh, position, glm::vec2(0.25f), VectorToAngle(velocity), sharedTexture), velocity(velocity)
{
}
Boid::~Boid()
{
}

void Boid::InitSharedResources(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
{
	sharedMesh = mesh;
	sharedTexture = texture;
}
