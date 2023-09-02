#pragma once
#include "GameRenderer/IGameRenderer.h"
#include "GameObjects/Stars.h"
#include "GameObjects/Stats.h"
#include "GameObjects/AI.h"

#include <memory>
#include <vector>
#include <chrono>
#include <optional>
#include <libhelpers/Dx/Renderer/IRenderer.h>

class SpaceGameRenderer final : public IRenderer {
public:
    SpaceGameRenderer(std::shared_ptr<GameRenderer::IGameRenderer> renderer);
    virtual ~SpaceGameRenderer();

    void Render() override;
    void OutputParametersChanged() override;

    void MouseMove(const Math::Vector2& pos) override;
    void MouseDown(const Math::Vector2& pos) override;
    void MouseUp(const Math::Vector2& pos) override;

private:
    static constexpr float GameFieldMainWidth = 3.2f;
    static constexpr float GameFieldMainHeight = 1.7f;

    void Update();

    Stats stats;
    AI ai;
    std::unique_ptr<Stars> stars;

    float lastDt = 0.f;
    std::optional<std::chrono::high_resolution_clock::time_point> prevTime;

    std::shared_ptr<GameRenderer::IGameRenderer> renderer;

    float rectRoundnessAngle = 0.f;

    std::shared_ptr<GameRenderer::IBackgroundBrushRenderer> bgBrush;
    std::shared_ptr<GameRenderer::IRectangleRenderer> testRect;
    std::shared_ptr<GameRenderer::ITextRenderer> testText;

    std::shared_ptr<GameRenderer::IRectangleRenderer> gameFieldMainBg;
    std::shared_ptr<GameRenderer::IRectangleRenderer> gameFieldMainFrame;
};
