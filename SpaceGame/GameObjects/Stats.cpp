#include "Stats.h"

void Stats::SetShieldColor(RGBA8Color shieldColor) {
    this->shieldColor = shieldColor;
}

void Stats::SetCaseColor(RGBA8Color caseColor) {
    this->caseColor = caseColor;
}

void Stats::Draw(GameRenderer::IGameRenderer& renderer) {
    this->UpdateShieldColor(renderer.GetFactory());
    this->UpdateCaseColor(renderer.GetFactory());

    if (!this->shipLivesMainBg
        || !this->shipLivesStateRectBg
        || this->shipLivesBgCrossHatchFill.empty()
        || !this->shipLivesStateRect
        || !this->shipLivesMainFrame)
    {
        return;
    }

    const Math::Vector2 scissorScale = { Stats::ShipLivesStateRectWidth, Stats::ShipLivesStateRectHeight };
    const Math::Vector2 scissorPos = { 0.f, 0.f };

    renderer.RenderRectangle(this->shipLivesMainBg);
    renderer.RenderRectangle(this->shipLivesStateRectBg);

    {
        auto scissorScope = renderer.PushScissorScoped(scissorPos, scissorScale);

        for (auto& i : this->shipLivesBgCrossHatchFill) {
            renderer.RenderRectangle(i);
        }
    }

    renderer.RenderRectangle(this->shipLivesStateRect);
    renderer.RenderRectangle(this->shipLivesMainFrame);
}

void Stats::UpdateShieldColor(GameRenderer::IGameRendererFactory& factory) {
    if (!this->shieldColor) {
        return;
    }

    {
        float startY = 0.f;
        float stepY = 0.02f;
        float height = 0.00001f;
        int lineCount = 40;

        this->shipLivesBgCrossHatchFill.clear();

        for (int i = 0; i < lineCount; i++) {
            auto rectRender = factory.MakeRectangleRenderer();
            auto rectRender2 = factory.MakeRectangleRenderer();

            GameRenderer::FilledRectangleGeometryParams rectGeom;

            rectGeom.height = height;
            rectGeom.color = *this->shieldColor;

            auto transform = rectRender->GetRectangleTransform();

            transform.position.y = (i * stepY) + ((lineCount / -2) * stepY);
            transform.rotation.z = DirectX::XMConvertToRadians(45.f);

            rectRender->SetGeometryParams(rectGeom);
            rectRender->SetRectangleTransform(transform);

            transform.rotation.z = DirectX::XMConvertToRadians(-45.f);

            rectRender2->SetGeometryParams(rectGeom);
            rectRender2->SetRectangleTransform(transform);

            this->shipLivesBgCrossHatchFill.push_back(std::move(rectRender));
            this->shipLivesBgCrossHatchFill.push_back(std::move(rectRender2));
        }
    }

    {
        this->shipLivesStateRect = factory.MakeRectangleRenderer();

        GameRenderer::HollowRectangleGeometryParams rectGeom;

        rectGeom.width = Stats::ShipLivesStateRectWidth;
        rectGeom.height = Stats::ShipLivesStateRectHeight;
        rectGeom.thickness = 0.005f;
        rectGeom.color = *this->shieldColor;

        this->shipLivesStateRect->SetGeometryParams(rectGeom);
    }

    {
        this->shipLivesMainBg = factory.MakeRectangleRenderer();

        GameRenderer::FilledRectangleGeometryParams rectGeom;

        rectGeom.width = Stats::ShipLivesMainWidth;
        rectGeom.height = Stats::ShipLivesMainHeight;
        rectGeom.color = RGBA8Color::RGBA8Color(0, 0, 0);

        this->shipLivesMainBg->SetGeometryParams(rectGeom);
    }

    {
        this->shipLivesMainFrame = factory.MakeRectangleRenderer();

        GameRenderer::HollowRectangleGeometryParams rectGeom;

        rectGeom.width = Stats::ShipLivesMainWidth;
        rectGeom.height = Stats::ShipLivesMainHeight;
        rectGeom.thickness = 0.005f;
        rectGeom.color = RGBA8Color::RGBA8Color(0, 0, 255);

        this->shipLivesMainFrame->SetGeometryParams(rectGeom);
    }

    this->shieldColor = {};
}

void Stats::UpdateCaseColor(GameRenderer::IGameRendererFactory& factory) {
    if (!this->caseColor) {
        return;
    }

    {
        this->shipLivesStateRectBg = factory.MakeRectangleRenderer();

        GameRenderer::FilledRectangleGeometryParams rectGeom;

        rectGeom.width = Stats::ShipLivesStateRectWidth;
        rectGeom.height = Stats::ShipLivesStateRectHeight;
        rectGeom.color = *this->caseColor;

        this->shipLivesStateRectBg->SetGeometryParams(rectGeom);
    }

    this->caseColor = {};
}
