#include "./Camera.h"

#include "../Entity.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const
{
	glm::mat4 transform;
	if (!hasTarget || !targetEntity) {
		transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
		transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(zoom, zoom, 1.0f));
	}
	else {
		transform = glm::translate(glm::mat4(1.0f), glm::vec3(targetEntity->position, 0.0f));
		transform = glm::rotate(transform, glm::radians(targetEntity->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(zoom, zoom, 1.0f));
	}
	return glm::inverse(transform);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	float halfWidth = (ORTHO_WIDTH / 2.0f) ;
	float halfHeight = (ORTHO_HEIGHT / 2.0f);
	return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
}

void Camera::ProcessMouseScroll(int direction)
{
	const float zoomStep = 0.1f;
	if (direction > 0) // scroll up
	{
		zoom -= zoomStep;
		if (zoom < 0.1f) zoom = 0.1f; // prevent zooming too close
	}
	else if (direction < 0) // scroll down
	{
		zoom += zoomStep;
		if (zoom > 10.0f) zoom = 10.0f;
	}
}
