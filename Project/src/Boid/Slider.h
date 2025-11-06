#pragma once

#include "../Engine/Entity.h"

class Slider : public Entity
{
public:
	Slider(std::shared_ptr<Mesh> mesh, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), float rotation = 0.0f, std::shared_ptr<Texture> texture = nullptr, std::shared_ptr<Shader> shader = nullptr,
		bool isGUI = false, float lower = 0.0f, float upper = 1.0f, float value = 0.0f);
	~Slider();

	float GetValue() const;
	void SetValue(float val);

	void ApplyUniforms(Shader& shader) const override;

	void BindControl(float& refVal, unsigned char keyLower, unsigned char keyUpper, float step = 0.1f);

private:
	float lowerBound;
	float upperBound;
	float value; // inbetween 0 and 1

};

