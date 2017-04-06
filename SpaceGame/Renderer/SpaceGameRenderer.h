#pragma once

#include <libhelpers\Dx\Renderer\IRenderer.h>

class SpaceGameRenderer : public IRenderer {
public:
    SpaceGameRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output);
    virtual ~SpaceGameRenderer();

    void Render() override;
    void OutputParametersChanged() override;

private:
};