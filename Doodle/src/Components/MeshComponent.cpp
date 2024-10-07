#include "MeshComponent.h"

namespace Doodle
{

void MeshComponent::Reload()
{
    switch (Info.Type)
    {
    case PrimitiveMeshType::Quad:
        Mesh = Mesh::GetQuad();
        return;
    case PrimitiveMeshType::Cube:
        Mesh = Mesh::GetCube();
        return;
    case PrimitiveMeshType::Sphere:
        Mesh = Mesh::GetSphere();
        return;
    case PrimitiveMeshType::Plane:
        Mesh = Mesh::GetPlane();
        return;
    case PrimitiveMeshType::None:
        break;
    }
    auto model = ResourceManager::Get()->Load<Model>(Info.ModelUUID);
    Mesh = model->GetMesh(Info.MeshIndex);
}

void MeshComponent::OnInspectorLayout()
{
    ImGuiUtils::ReadOnlyInputInt("Vertices", Mesh->GetVertexCount());
    ImGuiUtils::ReadOnlyInputInt("Faces", Mesh->GetFaceCount());
}

rfl::Generic::Object MeshComponent::SerializeToObject() const
{
    return rfl::json::read<rfl::Generic::Object>(rfl::json::write(Info)).value();
}
void MeshComponent::DeserializeFromObject(const rfl::Generic::Object &object)
{
    Info = rfl::json::read<MeshInfo>(rfl::json::write(object)).value();
    Reload();
}
} // namespace Doodle
