#pragma once

#include "glm/fwd.hpp"
#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "MaterialInstance.h"
#include "Mesh.h"
#include "SceneCamera.h"
#include "VertexArray.h"


namespace Doodle
{

struct Transform
{
    glm::vec3 Position;
    glm::vec3 Rotation; // Euler angles in degrees
    glm::vec3 Scale;

    Transform() : Position(0.0f), Rotation(0.0f), Scale(1.0f)
    {
    }

    glm::quat GetRotation() const
    {
        return glm::quat(glm::radians(Rotation));
    }

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model *= glm::toMat4(glm::quat(glm::radians(Rotation))); // Use quaternion for rotation
        model = glm::scale(model, Scale);
        return model;
    }

    glm::vec3 GetFront() const
    {
        glm::vec3 front(0.0f, 0.0f, -1.0f); // Assuming -Z is front
        glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
        return glm::normalize(glm::vec3(rotation * glm::vec4(front, 1.0f)));
    }

    glm::vec3 GetRight() const
    {
        glm::vec3 front = GetFront();
        glm::vec3 up(0.0f, 1.0f, 0.0f); // Assuming Y is up
        return glm::normalize(glm::cross(front, up));
    }

    glm::vec3 GetUp() const
    {
        glm::vec3 front = GetFront();
        glm::vec3 right = GetRight();
        return glm::normalize(glm::cross(right, front));
    }

    void LookAt(const glm::vec3 &target)
    {
        glm::vec3 direction = glm::normalize(target - Position);
        Rotation.x = glm::degrees(asin(direction.y));
        Rotation.y = glm::degrees(atan2(direction.z, direction.x));
        Rotation.z = 0.0f; // Reset roll or handle it as needed
    }

    void Rotate(const glm::vec3 &eulerAngles)
    {
        glm::quat currentRotation = glm::quat(glm::radians(Rotation));
        glm::quat additionalRotation = glm::quat(glm::radians(eulerAngles));
        Rotation = glm::degrees(glm::eulerAngles(currentRotation * additionalRotation));
    }

    void Translate(const glm::vec3 &translation)
    {
        Position += translation;
    }

    void ScaleBy(const glm::vec3 &scale)
    {
        Scale *= scale;
    }

    void ScaleBy(float scale)
    {
        Scale *= scale;
    }

    void Reset()
    {
        Position = glm::vec3(0.0f);
        Rotation = glm::vec3(0.0f);
        Scale = glm::vec3(1.0f);
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

struct DirectionalLightComponent
{
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
};

struct PointLightComponent
{
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float MinRadius = 0.001f;
    float Radius = 25.0f;
    float Falloff = 1.f;
    float SourceSize = 0.1f;
};

struct SpotLightComponent
{
    glm::vec3 Radiance{1.0f};
    float Intensity = 0.0f;
    float AngleAttenuation = 0.0f;
    float Range = 0.1f;
    float Angle = 0.0f;
    float Falloff = 1.0f;
};
} // namespace Doodle