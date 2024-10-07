#pragma once

#include <rfl/Variant.hpp>

#include "Asset.h"
#include "ISerializable.h"
#include "MaterialInfo.h"
#include "UUID.h"

namespace Doodle
{

class MaterialAsset : public NativeAsset, public ISerializable<MaterialInfo>
{
    friend class Material;

public:
    ASSET_TYPE(Scene)
    ASSET_EXTENSION(".dmat")
};

} // namespace Doodle