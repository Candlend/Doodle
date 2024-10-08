#pragma once

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "Mesh.h"
#include "SceneCamera.h"
#include "VertexArray.h"
#include "imgui.h"

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

    VAOComponent(std::shared_ptr<VertexArray> vao) : VAO(vao)
    {
    }
};

struct MeshComponent : public IRenderable
{
    COMPONENT_CLASS_TYPE(Mesh)

    std::shared_ptr<Mesh> Mesh;

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
};

} // namespace Doodle