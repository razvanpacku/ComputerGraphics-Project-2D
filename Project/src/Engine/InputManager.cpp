#include "./InputManager.h"

std::unordered_map<unsigned char, std::vector<InputManager::KeyCallback>> InputManager::keyCallbacks;
std::vector<std::function<void(int direction)>> InputManager::mouseWheelCallbacks;

void InputManager::RegisterKeyAction(unsigned char key, const KeyCallback& callback)
{
	keyCallbacks[key].push_back(callback);
}

void InputManager::RegisterMouseWheelAction(const std::function<void(int direction)>& callback)
{
	mouseWheelCallbacks.push_back(callback);
}

void InputManager::OnKeyPress(unsigned char key)
{
	auto it = keyCallbacks.find(key);
	if (it != keyCallbacks.end())
	{
		for (const auto& callback : it->second)
		{
			callback();
		}
	}
}

void InputManager::OnMouseWheel(int direction)
{
	for (const auto& callback : mouseWheelCallbacks)
	{
		callback(direction);
	}
}