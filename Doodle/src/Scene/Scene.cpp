#include "Scene.h"

namespace Doodle
{

std::shared_ptr<Scene> Scene::Create()
{
    return std::make_shared<Scene>();
}
Entity Scene::CreateEntity()
{
    auto entity = Entity(m_registry);
    entity.AddComponent<Transform>();
    return entity;
}

void Scene::Render() const
{
    auto vaoView = m_registry.view<Transform, VAOComponent, MaterialComponent>();
    for (auto entity : vaoView)
    {
        const auto &transform = vaoView.get<Transform>(entity);
        const auto &vao = vaoView.get<VAOComponent>(entity);
        const auto &material = vaoView.get<MaterialComponent>(entity);

        material.MaterialInstance->Bind();
        vao.VAO->Render();
    }

    auto meshView = m_registry.view<Transform, MeshComponent, MaterialComponent>();
    for (auto entity : meshView)
    {
        const auto &transform = meshView.get<Transform>(entity);
        const auto &mesh = meshView.get<MeshComponent>(entity);
        const auto &material = meshView.get<MaterialComponent>(entity);

        material.MaterialInstance->Bind();
        mesh.Mesh->Render();
    }
}

} // namespace Doodle