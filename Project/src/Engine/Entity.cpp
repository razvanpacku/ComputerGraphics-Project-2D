#include "./Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh, const glm::vec2& position, const glm::vec2& scale, float rotation, std::shared_ptr<Texture> texture, std::shared_ptr<Shader> shader, bool isGUI)
	: mesh(mesh), position(position), scale(scale), rotation(rotation), texture(texture), shader(shader), isGUI(isGUI)
{
}

Entity::~Entity()
{
}

glm::mat4 Entity::GetModelMatrix() const
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));
	return model;
}

void Entity::ApplyUniforms(Shader& shader) const{
    shader.setMat4("model", GetModelMatrix());
    shader.setBool("useTexture", useTexture);
}

void Entity::Draw() const
{
    // Apply texture if available
    if (useTexture && texture)
    {
        texture->Bind();
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

	mesh->Draw();

    if (texture)
        glBindTexture(GL_TEXTURE_2D, 0);
}
