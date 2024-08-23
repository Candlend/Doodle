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
        glm::mat4 view(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

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