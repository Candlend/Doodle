#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <string>

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