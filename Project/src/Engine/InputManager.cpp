#include "./InputManager.h"

std::unordered_map<unsigned char, std::vector<InputManager::KeyCallback>> InputManager::keyCallbacks;
std::vector<std::function<void(int direction)>> InputManager::mouseWheelCallbacks;
std::vector<std::function<void(int button, int state, int x, int y)>> InputManager::mouseButtonCallbacks;
std::vector<std::function<void(int x, int y)>> InputManager::cursorPosCallbacks;

void InputManager::RegisterKeyAction(unsigned char key, const KeyCallback& callback)
{
	keyCallbacks[key].push_back(callback);
}

void InputManager::RegisterMouseWheelAction(const std::function<void(int direction)>& callback)
{
	mouseWheelCallbacks.push_back(callback);
}

void InputManager::RegisterMouseButtonAction(const std::function<void(int button, int state, int x, int y)>& callback)
{
	mouseButtonCallbacks.push_back(callback);
}

void InputManager::RegisterCursorPosAction(const std::function<void(int x, int y)>& callback)
{
	cursorPosCallbacks.push_back(callback);
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

void InputManager::OnMouseButton(int button, int state, int x, int y)
{
	for (const auto& callback : mouseButtonCallbacks)
	{
		callback(button, state, x, y);
	}
}

void InputManager::OnCursorPos(int x, int y)
{
	for (const auto& callback : cursorPosCallbacks)
	{
		callback(x, y);
	}
}