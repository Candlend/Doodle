#pragma once

#include "pch.h"

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "Mesh.h"
#include "VertexArray.h"

namespace Doodle
{

struct IRenderable : public BaseComponent
{
    virtual void Render() const = 0;
};

struct VAOComponent : public IRenderable
{
    COMPONENT_CLASS_TYPE(VAO)

    std::shared_ptr<VertexArray> VAO;

    void Render() const override
    {
        VAO->Render();
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputInt("Renderer ID", VAO->GetRendererID());
    }

    VAOComponent() = default;
    VAOComponent(std::shared_ptr<VertexArray> vao) : VAO(vao)
    {
    }

    rfl::Generic::Object SerializeToObject() const override
    {
        rfl::Generic::Object object;
        // TODO: Serialize
        return object;
    }

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        // TODO: Deserialize
    }
};

struct MeshComponent : public IRenderable
{
    COMPONENT_CLASS_TYPE(Mesh)

    std::shared_ptr<Mesh> Mesh;

    MeshComponent() = default;
    MeshComponent(const std::string &filename) : Mesh(Mesh::Create(filename))
    {
    }

    MeshComponent(std::shared_ptr<Doodle::Mesh> mesh) : Mesh(mesh)
    {
    }

    void Render() const override
    {
        Mesh->Render();
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputInt("Vertices", Mesh->GetVertexCount());
        ImGuiUtils::ReadOnlyInputInt("Faces", Mesh->GetFaceCount());
    }

    rfl::Generic::Object SerializeToObject() const override
    {
        rfl::Generic::Object object;
        // TODO: Serialize
        return object;
    }

    void DeserializeFromObject(const rfl::Generic::Object &object) override
    {
        // TODO: Deserialize
    }
};

} // namespace Doodle
