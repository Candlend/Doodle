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
    std::shared_ptr<VertexArray> VAO;

    void Render() const override
    {
        VAO->Render();
    }

    std::string GetName() const override
    {
        return "VAO";
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputText("Renderer ID", "{}", VAO->GetRendererID());
    }
};

struct MeshComponent : public IRenderable
{
    std::shared_ptr<Mesh> Mesh;

    MeshComponent(const std::string &filename) : Mesh(Mesh::Create(filename))
    {
    }

    void Render() const override
    {
        Mesh->Render();
    }

    std::string GetName() const override
    {
        return "Mesh";
    }

    void OnInspectorLayout() override
    {
        ImGuiUtils::ReadOnlyInputText("Mesh Path", "{}", Mesh->GetPath().c_str());
    }
};

} // namespace Doodle