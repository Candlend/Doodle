#pragma once

#include "Entity.h"
#include "UUID.h"

namespace Doodle
{

enum class SelectionContext
{
    Global = 0,
    Scene,
};

class UUID;
class Entity;

class DOO_API SelectionManager
{
public:
    static void Select(SelectionContext context, UUID selectionID);
    static bool IsSelected(UUID selectionID);
    static bool IsSelected(SelectionContext context, UUID selectionID);
    static bool IsEntityOrAncestorSelected(Entity entity);
    static bool IsEntityOrAncestorSelected(SelectionContext context, Entity entity);
    static void Deselect(UUID selectionID);
    static void Deselect(SelectionContext context, UUID selectionID);
    static void DeselectAll();
    static void DeselectAll(SelectionContext context);
    static UUID GetSelection(SelectionContext context, size_t index);

    static size_t GetSelectionCount(SelectionContext contextID);
    inline static const std::vector<UUID> &GetSelections(SelectionContext context)
    {
        return s_Contexts[context];
    }

private:
    inline static std::unordered_map<SelectionContext, std::vector<UUID>> s_Contexts;
};

} // namespace Doodle