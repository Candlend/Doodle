#include "Scene.h"
#include "Entity.h"
#include "SceneManager.h"
#include <memory>

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

std::shared_ptr<Entity> Scene::GetMainCameraEntity()
{
    auto cameraView = m_registry.view<CameraComponent>();
    for (auto entity : cameraView)
    {
        const auto &camera = cameraView.get<CameraComponent>(entity);
        if (camera.Primary)
        {
            return std::make_shared<Entity>(m_registry, entity);
        }
    }
    DOO_CORE_WARN("No primary camera found");
    return nullptr;
}

void Scene::Render()
{
    auto cameraEntity = GetMainCameraEntity();
    if (!cameraEntity)
    {
        return;
    }
    glm::mat4 view = glm::inverse(cameraEntity->GetComponent<Transform>().GetModelMatrix());
    glm::mat4 projection = cameraEntity->GetComponent<CameraComponent>().GetProjectionMatrix();

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
        vao.Render();
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
        mesh.Render();
    }
}

void Scene::Begin()
{
    SceneManager::Get().m_activeScene = shared_from_this();
}

void Scene::End()
{
    SceneManager::Get().m_activeScene = nullptr;
}

} // namespace Doodle