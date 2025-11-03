#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ORTHO_WIDTH 10.0f
#define ORTHO_HEIGHT 10.0f

//forward declaration
class Entity;

class Camera
{
public:
	Camera();
	~Camera();
	
	glm::vec2 position = glm::vec2(0.0f);
	float rotation = 0.0f;

	float zoom = 1.0f;

	bool hasTarget = false;
	Entity* targetEntity = nullptr;

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

	void ProcessMouseScroll(int direction);
private:
};

