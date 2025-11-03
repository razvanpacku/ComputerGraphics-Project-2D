#include "./Window.h"

#include <iostream>

Window::Window(uint16_t width, uint16_t height, const std::string& name, int argc, char** argv)
	: width(width), height(height)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow(name.c_str());
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
		exit(1);
	}
}

Window::~Window()
{
}
