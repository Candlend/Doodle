#include <memory>
#include <rfl/json/write.hpp>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "LightComponent.h"
#include "Scene.h"
#include "SceneAsset.h"
#include "SceneManager.h"
#include "UUID.h"

namespace Doodle
{

std::shared_ptr<Scene> SceneManager::LoadScene(std::shared_ptr<SceneAsset> sceneAsset)
{
    if (sceneAsset->IsLoaded())
    {
        DOO_CORE_WARN("Scene already loaded");
        return m_activeScene;
    }
    if (m_activeScene)
    {
        m_activeScene->EndScene();
    }
    m_activeScene = std::make_shared<Scene>(sceneAsset);

    m_activeScene->BeginScene();
    return m_activeScene;
}

void SceneManager::SaveScene(const std::filesystem::path &filepath)
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return;
    }
    m_activeScene->SaveAsset(filepath);
}

} // namespace Doodle