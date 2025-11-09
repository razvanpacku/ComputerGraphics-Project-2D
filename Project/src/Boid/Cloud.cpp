#include "./Cloud.h"

std::shared_ptr<Mesh> Cloud::sharedMesh = nullptr;
std::vector<std::shared_ptr<Texture>> Cloud::sharedTextures = {};

template <typename T>
T RandomElement (const std::vector<T>& vec) {
	return vec[rand() % vec.size()];
}

Cloud::Cloud(const glm::vec2& position, const glm::vec2& scale, float opacity, float height, float leftBound, float rightBound)
	: Entity(sharedMesh, position, scale, static_cast<float>(rand() % 360), RandomElement(sharedTextures)), height(height), leftBound(leftBound), rightBound(rightBound)
{
	this->opacity = opacity;
}

Cloud::~Cloud()
{
}

void Cloud::InitSharedResources(std::shared_ptr<Mesh> mesh, std::vector<std::shared_ptr<Texture>> textures)
{
	sharedMesh = mesh;
	sharedTextures = textures;
}

void Cloud::Update(float deltaTime)
{
	// Move cloud horizontally based on its height (higher clouds move faster)
	position.x += height * CLOUD_SPEED_FACTOR * deltaTime;
	// Wrap-around logic
	if (position.x - scale.x * 0.5f > rightBound) {
		position.x = leftBound - scale.x * 0.5f;
	}
}
