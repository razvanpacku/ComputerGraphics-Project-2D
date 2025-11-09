#include "./Slider.h"

#include "../Engine/InputManager.h"

Slider::Slider(std::shared_ptr<Mesh> mesh, const glm::vec2& position, const glm::vec2& scale, float rotation, std::shared_ptr<Texture> texture, std::shared_ptr<Shader> shader,
	bool isGUI, float lower, float upper, float value) : Entity(mesh, position, scale, rotation, texture, shader, isGUI)
{
	if (upper < lower) {
		std::swap(lower, upper);
	}
	lowerBound = lower;
	upperBound = upper;
	this->value = (lowerBound == upperBound) ? 0 : (glm::clamp(value, lowerBound, upperBound) - lowerBound) / (upperBound - lowerBound);
}

Slider::~Slider(){
}

float Slider::GetValue() const {
	return (lowerBound == upperBound) ? lowerBound : value * (upperBound - lowerBound) + lowerBound;
}

void Slider::SetValue(float val) {
	value = (lowerBound == upperBound) ? 0 : (glm::clamp(val, lowerBound, upperBound) - lowerBound) / (upperBound - lowerBound);
}

void Slider::ApplyUniforms(Shader& shader) const {
	Entity::ApplyUniforms(shader);

	shader.setFloat("sliderValue", value);

}

void Slider::BindControl(float& refVal, unsigned char keyLower, unsigned char keyUpper, float step) {
	InputManager::RegisterKeyAction(keyLower, [&refVal, this, step]() {
		refVal -= step;
		refVal = glm::clamp(refVal, lowerBound, upperBound);
		SetValue(refVal);
		});
	InputManager::RegisterKeyAction(keyUpper, [&refVal, this, step]() {
		refVal += step;
		refVal = glm::clamp(refVal, lowerBound, upperBound);
		SetValue(refVal);
		});

	//set value to the reference so there is no potential discrepancy initially
	SetValue(refVal);
	//
}