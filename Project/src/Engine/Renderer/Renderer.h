#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#define DEFAULT_CLEAR_COLOR_R 0.75f
#define DEFAULT_CLEAR_COLOR_G 1.0f
#define DEFAULT_CLEAR_COLOR_B 1.0f

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Init() const;

	void Clear() const;
	
	void Render() const;

	void Cleanup() const;

	//temporary static variables, move to another file later
	static GLuint
		VaoId,
		VboId,
		ColorBufferId,
		ProgramId;
private:
};

