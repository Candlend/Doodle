#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MaterialInstance.h"
#include "Mesh.h"
#include "VertexArray.h"


namespace Doodle
{

struct Transform
{
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;

    Transform() : Position(0.0f), Rotation(0.0f), Scale(1.0f)
    {
    }

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, Scale);
        return model;
    }
};

struct VAOComponent
{
    std::shared_ptr<VertexArray> VAO;
};

struct MeshComponent
{
    std::shared_ptr<Mesh> Mesh;

    explicit MeshComponent(const std::string &filename) : Mesh(Mesh::Create(filename))
    {
    }
};

struct MaterialComponent
{
    std::shared_ptr<MaterialInstance> MaterialInstance;

    explicit MaterialComponent(const std::shared_ptr<Doodle::MaterialInstance> &materialInstance)
        : MaterialInstance(materialInstance)
    {
    }

    explicit MaterialComponent(const std::shared_ptr<Material> &material)
        : MaterialInstance(MaterialInstance::Create(material))
    {
    }
};

} // namespace Doodle