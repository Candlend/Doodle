#include "Scene.h"
#include "Entity.h"

namespace Doodle
{

std::shared_ptr<Scene> Scene::Create()
{
    return std::make_shared<Scene>();
}

std::shared_ptr<Entity> Scene::CreateEntity(const std::string &name)
{
    if (m_entities.find(name) != m_entities.end())
    {
        DOO_CORE_WARN("Entity with name {0} already exists", name);
    }
    auto entity = Entity::Create(m_registry);
    m_entities[name] = entity;
    return entity;
}

std::shared_ptr<Entity> Scene::GetEntity(const std::string &name) const
{
    return m_entities.at(name);
}

void Scene::Render(const std::shared_ptr<Camera> &camera)
{
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix(16.0f / 9.0f);

    auto vaoView = m_registry.view<Transform, VAOComponent, MaterialComponent>();
    for (auto entity : vaoView)
    {
        const auto &transform = vaoView.get<Transform>(entity);
        const auto &vao = vaoView.get<VAOComponent>(entity);
        const auto &material = vaoView.get<MaterialComponent>(entity);

        glm::mat4 model = transform.GetModelMatrix();

        material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
        material.MaterialInstance->SetUniformMatrix4f("u_View", view);
        material.MaterialInstance->SetUniformMatrix4f("u_Projection", projection);

        material.MaterialInstance->Bind();
        vao.VAO->Render();
    }

    auto meshView = m_registry.view<Transform, MeshComponent, MaterialComponent>();
    for (auto entity : meshView)
    {

        const auto &transform = meshView.get<Transform>(entity);
        const auto &mesh = meshView.get<MeshComponent>(entity);
        const auto &material = meshView.get<MaterialComponent>(entity);

        glm::mat4 model = transform.GetModelMatrix();

        material.MaterialInstance->SetUniformMatrix4f("u_Model", model);
        material.MaterialInstance->SetUniformMatrix4f("u_View", view);
        material.MaterialInstance->SetUniformMatrix4f("u_Projection", projection);

        material.MaterialInstance->Bind();
        mesh.Mesh->Render();
    }
}

} // namespace Doodle