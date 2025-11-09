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

	// Register mouse button callback: (button, state, x, y)
	static void RegisterMouseButtonAction(const std::function<void(int button, int state, int x, int y)>& callback);

	// Register cursor position callback: (x, y)
	static void RegisterCursorPosAction(const std::function<void(int x, int y)>& callback);

	// Called by GLUT callbacks
	static void OnKeyPress(unsigned char key);
	static void OnMouseWheel(int direction);
	static void OnMouseButton(int button, int state, int x, int y);
	static void OnCursorPos(int x, int y);
private:
	static std::unordered_map<unsigned char, std::vector<KeyCallback>> keyCallbacks;
	static std::vector<std::function<void(int direction)>> mouseWheelCallbacks;
	static std::vector<std::function<void(int button, int state, int x, int y)>> mouseButtonCallbacks;
	static std::vector<std::function<void(int x, int y)>> cursorPosCallbacks;
};

