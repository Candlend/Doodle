#include "LayerStack.h"

namespace RhyEngine
{
LayerStack::LayerStack()
{
    m_layerInsert = m_layers.begin();
}

LayerStack::~LayerStack()
{
    for (BaseLayer *layer : m_layers)
        delete layer;
}

void LayerStack::PushLayer(BaseLayer *layer)
{
    m_layerInsert = m_layers.emplace(m_layerInsert, layer);
}

void LayerStack::PushOverlay(BaseLayer *overlay)
{
    m_layers.emplace_back(overlay);
}

void LayerStack::Polayer(BaseLayer *layer)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
        m_layerInsert--;
    }
}

void LayerStack::PopOverlay(BaseLayer *overlay)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
    if (it != m_layers.end())
        m_layers.erase(it);
}

} // namespace RhyEngine