#include "./Renderer.h"

//temporary functions for rendering, move to another file later
GLuint Renderer::VboId = 0;
GLuint Renderer::VaoId = 0;
GLuint Renderer::ColorBufferId = 0;
GLuint Renderer::ProgramId = 0;

#include "Util/loadShaders.h"
void CreateVBO(void)
{
	GLfloat Vertices[] = {
		0.5f,  0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f,
		0.5f,  0.5f, 0.0f, 1.0f
	};

	GLfloat Colors[] = {
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	};

	glGenBuffers(1, &Renderer::VboId);
	glBindBuffer(GL_ARRAY_BUFFER, Renderer::VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &Renderer::VaoId);
	glBindVertexArray(Renderer::VaoId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &Renderer::ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, Renderer::ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}
void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &Renderer::ColorBufferId);
	glDeleteBuffers(1, &Renderer::VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &Renderer::VaoId);
}

void CreateShaders(void)
{
	Renderer::ProgramId = LoadShaders("shaders/base.vert", "shaders/base.frag");
	glUseProgram(Renderer::ProgramId);
}
void DestroyShaders(void)
{
	glDeleteProgram(Renderer::ProgramId);
}

Renderer::Renderer()
{
	Init();
}

Renderer::~Renderer()
{
}

void Renderer::Init() const
{
	static Renderer* self = (Renderer*)this;

	glClearColor(DEFAULT_CLEAR_COLOR_R, DEFAULT_CLEAR_COLOR_G, DEFAULT_CLEAR_COLOR_B, 1.0f);

	glutDisplayFunc([]() {
		self->Render();
		});
	glutCloseFunc([]() {
		self->Cleanup();
		});

	CreateVBO();
	CreateShaders();
}

void Renderer::Clear() const
{
	glClearColor(DEFAULT_CLEAR_COLOR_R, DEFAULT_CLEAR_COLOR_G, DEFAULT_CLEAR_COLOR_B, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Render() const
{
	Clear();

	//do rendering here
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glFlush();
}

void Renderer::Cleanup() const
{
	//cleanup resources here
	DestroyShaders();
	DestroyVBO();
}