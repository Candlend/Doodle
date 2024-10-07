#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "SceneInfo.h"
#include "UUID.h"

namespace Doodle
{

class SceneAsset : public NativeAsset, public ISerializable<SceneInfo>
{
    friend class Scene;

public:
    ASSET_TYPE(Scene)
    ASSET_EXTENSION(".dscene")
};

} // namespace Doodle