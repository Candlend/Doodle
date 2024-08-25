#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MaterialInstance.h"
#include "Mesh.h"
#include "SceneCamera.h"
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

struct IRenderable
{
    virtual void Render() const = 0;
};

struct VAOComponent : public IRenderable
{
    std::shared_ptr<VertexArray> VAO;

    void Render() const override
    {
        VAO->Render();
    }
};

struct MeshComponent : public IRenderable
{
    std::shared_ptr<Mesh> Mesh;

    explicit MeshComponent(const std::string &filename) : Mesh(Mesh::Create(filename))
    {
    }

    void Render() const override
    {
        Mesh->Render();
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

struct CameraComponent
{
    std::shared_ptr<SceneCamera> Camera;
    bool Primary = true;

    explicit CameraComponent(ProjectionType type = ProjectionType::Perspective)
        : Camera(std::make_shared<SceneCamera>(type))
    {
    }
    
    glm::mat4 GetProjectionMatrix() const
    {
        return Camera->GetProjectionMatrix();
    }
};

} // namespace Doodle