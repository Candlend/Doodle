#pragma once

#include <boost/algorithm/string.hpp>

#include "FileSystem.h"
#include "UUID.h"

namespace Doodle
{

class DOO_API Asset : public std::enable_shared_from_this<Asset>
{
public:
    virtual void OnInspectorLayout() {};
    virtual std::string GetAssetType() const = 0;
    virtual std::string GetAssetExtension() const = 0;
    virtual void Reload() = 0;
    virtual UUID GetUUID() const = 0;
    virtual void Save() = 0;
    virtual void SaveAs(const std::filesystem::path &filepath) = 0;
    virtual bool Load(const std::filesystem::path &filepath) = 0;

    std::filesystem::path GetAssetPath() const
    {
        return m_assetPath;
    }

    bool IsLoaded() const
    {
        return m_loaded;
    }

protected:
    bool m_loaded = false;
    std::filesystem::path m_assetPath;
};

class DOO_API NativeAsset : public Asset
{
};

class DOO_API FileAsset : public Asset
{
public:
    virtual std::vector<FileDialogFilterItem> GetFileFilters() const = 0;

    FileAsset(const std::filesystem::path &filepath)
    {
        m_bindFilename = filepath.filename().string();
    }

    bool IsValid() const
    {
        if (FileSystem::Exists(GetBindFilepath()))
        {
            return true;
        }
        return false;
    }

    std::filesystem::path GetBindFilepath() const
    {
        return m_assetPath.parent_path() / m_bindFilename;
    }

protected:
    std::string m_bindFilename;
};

#define ASSET_TYPE(type)                                                                                               \
    static std::string GetStaticType()                                                                                 \
    {                                                                                                                  \
        return #type;                                                                                                  \
    }                                                                                                                  \
    virtual std::string GetAssetType() const override                                                                  \
    {                                                                                                                  \
        return GetStaticType();                                                                                        \
    }

#define ASSET_EXTENSION(ext)                                                                                           \
    static std::string GetStaticExtension()                                                                            \
    {                                                                                                                  \
        return ext;                                                                                                    \
    }                                                                                                                  \
    virtual std::string GetAssetExtension() const override                                                             \
    {                                                                                                                  \
        return GetStaticExtension();                                                                                   \
    }

#define BIND_FILE_FILTER(...)                                                                                          \
    static std::vector<FileDialogFilterItem> GetStaticFileFilters()                                                    \
    {                                                                                                                  \
        return {__VA_ARGS__};                                                                                          \
    }                                                                                                                  \
    virtual std::vector<FileDialogFilterItem> GetFileFilters() const override                                          \
    {                                                                                                                  \
        return GetStaticFileFilters();                                                                                 \
    }                                                                                                                  \
    static bool IsSupported(const std::filesystem::path &filepath)                                                     \
    {                                                                                                                  \
        for (const auto &filter : GetStaticFileFilters())                                                              \
        {                                                                                                              \
            std::vector<std::string> supportedExtensions;                                                              \
            boost::split(supportedExtensions, filter.m_spec, boost::is_any_of(","));                                   \
            for (const auto &ext : supportedExtensions)                                                                \
            {                                                                                                          \
                if (filepath.string().ends_with("." + ext))                                                            \
                {                                                                                                      \
                    return true;                                                                                       \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
        return false;                                                                                                  \
    }                                                                                                                  \
    static std::filesystem::path GetAssetPathByFilepath(const std::filesystem::path &filepath)                         \
    {                                                                                                                  \
        DOO_CORE_ASSERT(IsSupported(filepath), "Invalid file extension: {0}", filepath.extension().string());          \
        auto directory = filepath.parent_path();                                                                       \
        return directory / (filepath.string() + GetStaticExtension());                                                 \
    }

} // namespace Doodle