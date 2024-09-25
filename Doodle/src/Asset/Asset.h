#pragma once

#include "UUID.h"

namespace Doodle
{

class DOO_API BaseAsset
{
public:
    virtual void OnInspectorLayout() {};
    virtual std::string GetAssetType() = 0;
    virtual std::string GetAssetExtension() = 0;
    virtual void Reload() = 0;
    virtual UUID GetUUID() const = 0;

    std::filesystem::path GetFilepath() const
    {
        return m_filepath;
    }

protected:
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

template <typename T> class DOO_API Asset : public BaseAsset
{
public:
    virtual std::shared_ptr<T> CreateInstance() = 0;
};

template <typename T> class DOO_API NativeAsset : public Asset<T>
{
};

} // namespace Doodle