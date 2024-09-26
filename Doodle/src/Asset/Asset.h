#pragma once

#include "UUID.h"

namespace Doodle
{

class DOO_API Asset : public std::enable_shared_from_this<Asset>
{
public:
    virtual void OnInspectorLayout() {};
    virtual std::string GetAssetType() = 0;
    virtual std::string GetAssetExtension() = 0;
    virtual void Reload() = 0;
    virtual UUID GetUUID() const = 0;
    virtual void Save() = 0;
    virtual void SaveAs(const std::filesystem::path &filepath) = 0;
    virtual bool Load(const std::filesystem::path &filepath) = 0;

    std::filesystem::path GetFilepath() const
    {
        return m_filepath;
    }

    bool IsLoaded() const
    {
        return m_loaded;
    }

protected:
    bool m_loaded = false;
    std::filesystem::path m_filepath;
};

#define ASSET_TYPE(type, extension)                                                                                    \
    static std::string GetStaticType()                                                                                 \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }                                                                                                                  \
    virtual std::string GetAssetType() override                                                                        \
    {                                                                                                                  \
        return GetStaticType();                                                                                        \
    }                                                                                                                  \
    static std::string GetStaticExtension()                                                                            \
    {                                                                                                                  \
        return #extension;                                                                                             \
    }                                                                                                                  \
    virtual std::string GetAssetExtension() override                                                                   \
    {                                                                                                                  \
        return GetStaticExtension();                                                                                   \
    }

template <typename T> class DOO_API NativeAsset : public Asset
{
};

} // namespace Doodle