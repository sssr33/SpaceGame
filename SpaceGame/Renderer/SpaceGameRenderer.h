#pragma once
#include "GameRenderer/IGameRenderer.h"

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

    float rectRoundnessAngle = 0.f;

    std::shared_ptr<GameRenderer::IBackgroundBrushRenderer> bgBrush;
    std::shared_ptr<GameRenderer::IRectangleRenderer> testRect;
    std::shared_ptr<GameRenderer::ITextRenderer> testText;
};