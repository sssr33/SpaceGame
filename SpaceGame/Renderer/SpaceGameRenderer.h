#pragma once
#include "GameRenderer/IGameRenderer.h"
#include "..\GameObjects\BackgroundBrush.h"
#include "..\GameObjects\RectRenderer.h"

#include <memory>
#include <libhelpers\Dx\Renderer\IRenderer.h>

class SpaceGameRenderer : public IRenderer {
public:
    SpaceGameRenderer(std::shared_ptr<GameRenderer::IGameRenderer> renderer);
    virtual ~SpaceGameRenderer();

    void Render() override;
    void OutputParametersChanged() override;

private:
    std::shared_ptr<GameRenderer::IGameRenderer> renderer;

    std::shared_ptr<GameRenderer::IBackgroundBrushRenderer> bgBrush;
    std::shared_ptr<GameRenderer::IRectangleRenderer> testRect;

    /*BackgroundBrush bgBrush;
    RectRenderer testRect;*/
};