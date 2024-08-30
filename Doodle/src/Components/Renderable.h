#pragma once

#include "Mesh.h"
#include "SceneCamera.h"
#include "VertexArray.h"

namespace Doodle
{

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

    MeshComponent(const std::string &filename) : Mesh(Mesh::Create(filename))
    {
    }

    void Render() const override
    {
        Mesh->Render();
    }
};

} // namespace Doodle