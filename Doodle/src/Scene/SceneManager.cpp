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
    m_activeScene = Scene::Load(sceneInfo);
    m_activeScene->BeginScene();
    return m_activeScene;
}

SceneInfo SceneManager::GetSceneInfo()
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return SceneInfo();
    }

    return m_activeScene->GetInfo();
}

void SceneManager::SaveScene(const std::string &filepath)
{
    if (!m_activeScene)
    {
        DOO_CORE_WARN("No active scene found");
        return;
    }
    auto sceneInfo = GetSceneInfo();
    auto uuid = sceneInfo.UUID;
    auto sceneAsset = AssetManager::Get()->GetAsset<SceneAsset>(uuid);
    sceneAsset->SetData(sceneInfo);
    if (filepath.empty())
    {
        sceneAsset->Save();
    }
    sceneAsset->SaveAs(filepath);
}

} // namespace Doodle