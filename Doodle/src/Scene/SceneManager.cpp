#include <memory>
#include <rfl/json/write.hpp>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "LightComponent.h"
#include "Scene.h"
#include "SceneAsset.h"
#include "SceneInfo.h"
#include "SceneManager.h"
#include "UUID.h"


namespace Doodle
{

std::shared_ptr<Scene> SceneManager::LoadScene(const std::filesystem::path &assetPath)
{
    if (m_activeScene)
    {
        m_activeScene->EndScene();
    }
    auto asset = AssetManager::Get()->GetAsset<SceneAsset>(assetPath);

    struct SceneWrapper : public Scene
    {
        SceneWrapper(const SceneInfo &info) : Scene(info)
        {
        }
    };

    m_activeScene = std::make_shared<SceneWrapper>(asset->GetInfo());

    m_activeScene->BeginScene();
    return m_activeScene;
}

void SceneManager::SaveScene(const std::filesystem::path &assetPath)
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return;
    }
    auto asset = AssetManager::Get()->TryGetAsset<SceneAsset>(assetPath);
    if (!asset)
    {
        asset = AssetManager::Get()->Create<SceneAsset>(assetPath);
    }
    auto info = m_activeScene->GetInfo();
    asset->SetInfo(info);
    asset->Save();
}

} // namespace Doodle