#include <magic_enum.hpp>

#include "TextureParams.h"

namespace Doodle
{

std::string TextureParams::ToString() const
{
    std::ostringstream oss;
    oss << "Format=" << magic_enum::enum_name(Format) << ", Wrap=" << magic_enum::enum_name(Wrap)
        << ", Filter=" << magic_enum::enum_name(Filter) << ", Width=" << Width << ", Height=" << Height;
    return oss.str();
}

} // namespace Doodle