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

class DOO_API SceneManager : public Singleton<SceneManager>
{
    friend class Scene;

public:
    std::shared_ptr<Scene> CreateScene(const std::string &name);
    void AddScene(const std::string &name, std::shared_ptr<Scene> scene);
    void RemoveScene(const std::string &name);
    std::shared_ptr<Scene> GetScene(const std::string &name) const;
    std::shared_ptr<Scene> GetActiveScene() const
    {
        return m_activeScene;
    }
    SceneState GetState() const
    {
        return m_state;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
    std::shared_ptr<Scene> m_activeScene;
    SceneState m_state = SceneState::Editor;
};

} // namespace Doodle