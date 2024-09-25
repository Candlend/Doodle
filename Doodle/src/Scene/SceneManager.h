#pragma once

#include "pch.h"

#include "Scene.h"
#include "SceneAsset.h"
#include "Singleton.h"

namespace Doodle
{

enum class SceneState
{
    Editor,
    Runtime,
};

class DOO_API SceneManager : public Singleton<SceneManager>
{
    friend class Scene;

public:
    std::shared_ptr<Scene> GetActiveScene() const
    {
        return m_activeScene;
    }
    SceneState GetState() const
    {
        return m_state;
    }
    std::shared_ptr<Scene> LoadScene(const SceneInfo &sceneInfo);
    SceneInfo GetSceneInfo();

    void SaveScene(const std::filesystem::path &filepath = "");

private:
    Entity DeserializeEntity(const EntityInfo &entityInfo);
    EntityInfo SerializeEntity(const Entity &entity);
    std::shared_ptr<Scene> m_activeScene;
    SceneInfo m_activeSceneInfo;
    SceneState m_state = SceneState::Editor;
};

} // namespace Doodle