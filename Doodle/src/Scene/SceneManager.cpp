#include <rfl/json/write.hpp>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "Scene.h"
#include "SceneAsset.h"
#include "SceneManager.h"

namespace Doodle
{

std::shared_ptr<Scene> SceneManager::LoadScene(const SceneInfo &sceneInfo)
{
    if (m_activeScene)
    {
        m_activeScene->EndScene();
    }
    m_activeScene = std::make_shared<Scene>();
    m_activeSceneInfo = sceneInfo;

    auto entities = sceneInfo.Entities;
    // TODO Load scene from sceneInfo
    auto entity = m_activeScene->CreateEntity("Test");
    DOO_CORE_DEBUG(rfl::json::write(entity.GetComponent<IDComponent>()));

    m_activeScene->BeginScene();
    return m_activeScene;
}

EntityInfo SceneManager::SerializeEntity(const Entity &entity)
{
    EntityInfo entityInfo;
    for (auto *component : m_activeScene->GetComponents(entity.GetUUID()))
    {
        // auto *comp = dynamic_cast<ComponentTypes *>(component);
        // if (comp)
        // {
        //     entityInfo.Components.push_back(*comp);
        // }
    }
    for (const auto &child : entity.GetChildren())
    {
        EntityInfo childInfo = SerializeEntity(child);
        entityInfo.Children.push_back(childInfo);
    }
    return entityInfo;
}

SceneInfo SceneManager::GetSceneInfo()
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return SceneInfo();
    }
    for (const auto &entity : m_activeScene->GetEntities())
    {
        EntityInfo entityInfo = SerializeEntity(entity);
        m_activeSceneInfo.Entities.push_back(entityInfo);
    }
    return m_activeSceneInfo;
}

void SceneManager::SaveScene(const std::filesystem::path &filepath)
{
    auto sceneInfo = GetSceneInfo();
    auto uuid = sceneInfo.UUID;
    auto sceneAsset = AssetManager::Get()->GetAsset<SceneAsset>(uuid);
    sceneAsset->SetData(sceneInfo);
    if (filepath.empty())
    {
        sceneAsset->Save();
    }
    else
    {
        sceneAsset->SaveAs(filepath);
    }
}

} // namespace Doodle