#include "PanelManager.h"
#include "EditorPanel.h"

namespace Doodle
{

PanelData *PanelManager::GetPanelData(uint32_t id)
{
    auto it = m_panelDataMap.find(id);
    if (it != m_panelDataMap.end())
    {
        return &it->second;
    }
    return nullptr;
}

void PanelManager::SetPanelData(uint32_t id, const PanelData &data)
{
    m_panelDataMap[id] = data;
}

} // namespace Doodle