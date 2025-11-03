#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <cstdint>
#include <string>

class Window
{
public:
	Window(uint16_t width, uint16_t height, const std::string& name, int argc, char** argv);
	~Window();

	uint16_t GetWidth() const { return width; }
	uint16_t GetHeight() const { return height; }
private:
	uint16_t width, height;

};

