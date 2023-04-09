#pragma once
#include "GameRenderer/IGameRenderer.h"
#include "GameObjects/Stars.h"

#include <memory>
#include <vector>
#include <chrono>
#include <optional>
#include <libhelpers/Dx/Renderer/IRenderer.h>

class SpaceGameRenderer : public IRenderer {
public:
    SpaceGameRenderer(std::shared_ptr<GameRenderer::IGameRenderer> renderer);
    virtual ~SpaceGameRenderer();

    void Render() override;
    void OutputParametersChanged() override;

private:
    static constexpr float GameFieldMainWidth = 3.2f;
    static constexpr float GameFieldMainHeight = 1.7f;
    static constexpr float ShipLivesMainWidth = 0.28f;
    static constexpr float ShipLivesMainHeight = 0.38f;
    static constexpr float ShipLivesStateRectWidth = 0.16f;
    static constexpr float ShipLivesStateRectHeight = 0.16f;

    void Update();

    std::unique_ptr<Stars> stars;

    std::optional<std::chrono::high_resolution_clock::time_point> prevTime;

    std::shared_ptr<GameRenderer::IGameRenderer> renderer;

    float rectRoundnessAngle = 0.f;

    std::shared_ptr<GameRenderer::IBackgroundBrushRenderer> bgBrush;
    std::shared_ptr<GameRenderer::IRectangleRenderer> testRect;
    std::shared_ptr<GameRenderer::ITextRenderer> testText;

    std::vector<std::shared_ptr<GameRenderer::IRectangleRenderer>> shipLivesBgCrossHatchFill;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesStateRect;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesMainBg;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesMainFrame;

    std::shared_ptr<GameRenderer::IRectangleRenderer> gameFieldMainBg;
    std::shared_ptr<GameRenderer::IRectangleRenderer> gameFieldMainFrame;
    std::vector<std::shared_ptr<GameRenderer::IRectangleRenderer>> gameFieldEnemyZoneLines;
};