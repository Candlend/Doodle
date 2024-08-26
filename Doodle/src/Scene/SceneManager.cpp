
#include "SceneManager.h"
#include <memory>

namespace Doodle
{

std::shared_ptr<Scene> SceneManager::CreateScene(const std::string &name)
{
    if (m_scenes.find(name) != m_scenes.end())
    {
        DOO_CORE_WARN("Scene with name {0} already exists", name);
    }
    auto scene = Scene::Create();
    m_scenes[name] = scene;
    return scene;
}

void SceneManager::AddScene(const std::string &name, std::shared_ptr<Scene> scene)
{
    m_scenes[name] = scene;
}

} // namespace Doodle