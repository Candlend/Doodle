#pragma once

#include "pch.h"

#include "Scene.h"
#include <memory>

namespace Doodle
{

enum class SceneState
{
    Editor,
    Runtime,
};

class SceneInfo;
class EntityInfo;
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

    void SaveScene(const std::string &filepath = "");

private:
    std::shared_ptr<Scene> m_activeScene;
    SceneState m_state = SceneState::Editor;
};

} // namespace Doodle