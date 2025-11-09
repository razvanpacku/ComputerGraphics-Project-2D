#pragma once
#include <GL/freeglut.h>

#include <functional>
#include <unordered_map>
#include <vector>

// static class for managing input
class InputManager
{
public:
	using KeyCallback = std::function<void()>;

	// Register a callback for a key press
	static void RegisterKeyAction(unsigned char key, const KeyCallback& callback);

	// Register mouse wheel callback
	static void RegisterMouseWheelAction(const std::function<void(int direction)>& callback);

	// Called by GLUT callbacks
	static void OnKeyPress(unsigned char key);
	static void OnMouseWheel(int direction);
private:
	static std::unordered_map<unsigned char, std::vector<KeyCallback>> keyCallbacks;
	static std::vector<std::function<void(int direction)>> mouseWheelCallbacks;
};

