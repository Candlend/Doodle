#pragma once

#include "pch.h"
#include <any>

#include "AssetManager.h"
#include "Singleton.h"
#include "UUID.h"

namespace Doodle
{

class ResourceManager : public Singleton<ResourceManager>
{
public:
    template <typename T> std::shared_ptr<T> Load(const std::string &path)
    {
        // 检查缓存
        std::shared_ptr<T> resource;
        auto asset = AssetManager::Get()->GetAsset(path);
        auto uuid = asset->GetUUID();

        if (m_resourceCache.find(uuid) != m_resourceCache.end())
        {
            // 返回已加载的实例
            resource = std::any_cast<std::shared_ptr<T>>(m_resourceCache[uuid]);
            return resource;
        }

        // 从磁盘加载资源
        resource = T::Load(path);

        // 存储到缓存
        m_resourceCache[uuid] = resource;

        return resource;
    }

    template <typename T> std::shared_ptr<T> Load(const UUID &uuid)
    {
        // 检查缓存
        std::shared_ptr<T> resource;

        if (m_resourceCache.find(uuid) != m_resourceCache.end())
        {
            // 返回已加载的实例
            resource = std::any_cast<std::shared_ptr<T>>(m_resourceCache[uuid]);
            return resource;
        }

        auto asset = AssetManager::Get()->GetAsset(uuid);
        // 从磁盘加载资源
        resource = T::Load(asset->GetAssetPath());

        // 存储到缓存
        m_resourceCache[uuid] = resource;

        return resource;
    }

    template <typename T> void Add(const UUID &uuid, const T &resource)
    {
        m_resourceCache[uuid] = resource;
    }

private:
    std::unordered_map<UUID, std::any> m_resourceCache;
};

} // namespace Doodle
