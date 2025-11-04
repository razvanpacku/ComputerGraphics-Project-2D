#pragma once
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <functional>

// Forward declaration
class Entity;
class Renderer;

class Scene
{
public:

    Scene(Renderer* renderer);
    virtual ~Scene(); 

    Entity* AddEntity(std::unique_ptr<Entity> entity);
    void RemoveEntity(std::unique_ptr<Entity> entity);

    void SetUpdateFunction(const std::function<void(float)>& func) {
        updateFunction = func;
    }
    void Update(float deltaTime);

    glm::vec3 backgroundColor = glm::vec3(0.75f, 1.0f, 1.0f);

    void Render() const;
protected:
    std::vector<std::unique_ptr<Entity>> entities;

    std::function<void(float)> updateFunction;
private:

    Renderer* renderer;
};

