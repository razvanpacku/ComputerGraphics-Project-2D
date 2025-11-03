#include "./Renderer.h"

//temporary functions for rendering, move to another file later

#include "Util/loadShaders.h"
void Renderer::CreateVBO(void)
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

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}
void Renderer::DestroyVBO(void) const
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void Renderer::CreateShaders(void)
{
	shader = new Shader("shaders/base.vert", "shaders/base.frag");
	ProgramId = shader->GetID();
	glUseProgram(ProgramId);
}
void Renderer::DestroyShaders(void) const
{
	glDeleteProgram(ProgramId);
}

Renderer::Renderer()
{
	Init();
}

Renderer::~Renderer()
{
	delete shader;
}

void Renderer::Init()
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