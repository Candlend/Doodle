#pragma once

#include "pch.h"

#include "BaseComponent.h"
#include "ImGuiUtils.h"
#include "Mesh.h"
#include "MeshInfo.h"
#include "Model.h"
#include "ResourceManager.h"
#include "VertexArray.h"
#include "rfl/json/write.hpp"

namespace Doodle
{

struct MeshComponent : public BaseComponent
{
    COMPONENT_CLASS_TYPE(Mesh)

    std::shared_ptr<Mesh> Mesh;
    MeshInfo Info;

    MeshComponent() = default;
    MeshComponent(const MeshInfo &info) : Info(info)
    {
        Reload();
    }

    void Reload();

    void Render() const
    {
        Mesh->Render();
    }

    void OnInspectorLayout() override;

    rfl::Generic::Object SerializeToObject() const override;

    void DeserializeFromObject(const rfl::Generic::Object &object) override;
};

} // namespace Doodle
