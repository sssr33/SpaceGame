#pragma once
#include "Renderer/GameRenderer/IGameRenderer.h"
#include "Renderer/GameRenderer/IRectangleRenderer.h"

#include <vector>
#include <optional>

class Stats {
public:
    static constexpr float ShipLivesMainWidth = 0.28f;
    static constexpr float ShipLivesMainHeight = 0.38f;
    static constexpr float ShipLivesStateRectWidth = 0.16f;
    static constexpr float ShipLivesStateRectHeight = 0.16f;
    static constexpr RGBA8Color ShieldColorGreen = RGBA8Color::RGBA8Color(0, 255, 0);
    static constexpr RGBA8Color CaseColorGreen = RGBA8Color::RGBA8Color(0, 200, 0);

    void SetShieldColor(RGBA8Color shieldColor);
    void SetCaseColor(RGBA8Color caseColor);

    void Draw(GameRenderer::IGameRenderer& renderer);

private:
    void UpdateShieldColor(GameRenderer::IGameRendererFactory& factory);
    void UpdateCaseColor(GameRenderer::IGameRendererFactory& factory);

    std::optional<RGBA8Color> shieldColor = Stats::ShieldColorGreen;
    std::optional<RGBA8Color> caseColor = Stats::CaseColorGreen;
    std::optional<RGBA8Color> shieldColorPrev;
    std::optional<RGBA8Color> caseColorPrev;

    std::vector<std::shared_ptr<GameRenderer::IRectangleRenderer>> shipLivesBgCrossHatchFill;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesStateRect;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesStateRectBg;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesMainBg;
    std::shared_ptr<GameRenderer::IRectangleRenderer> shipLivesMainFrame;
};
