#pragma once

#include "Layer.h"
#include "Core.h"


namespace RhyEngine
{

class RHY_API LayerStack
{
public:
    LayerStack();
    ~LayerStack();

    void PushLayer(BaseLayer *layer);
    void PushOverlay(BaseLayer *overlay);
    void Polayer(BaseLayer *layer);
    void PopOverlay(BaseLayer *overlay);

    std::vector<BaseLayer *>::iterator begin()
    {
        return m_layers.begin();
    }
    std::vector<BaseLayer *>::iterator end()
    {
        return m_layers.end();
    }

private:
    std::vector<BaseLayer *> m_layers;
    std::vector<BaseLayer *>::iterator m_layerInsert;
};

} // namespace RhyEngine