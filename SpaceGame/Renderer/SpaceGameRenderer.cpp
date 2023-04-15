#include "SpaceGameRenderer.h"

#include <libhelpersDesktop/Filesystem/StreamFILE.h>
#include <cmath>
#include <libhelpers/Math/Matrix.h>

SpaceGameRenderer::SpaceGameRenderer(std::shared_ptr<GameRenderer::IGameRenderer> renderer)
    : renderer(std::move(renderer))
{
    auto opScope = this->renderer->OperationBeginScoped();
    auto& rendererFactory = this->renderer->GetFactory();

    this->bgBrush = rendererFactory.MakeBackgroundBrushRenderer(L"Assets\\back.bmp");
    this->testText = rendererFactory.MakeTextRenderer(L"Arial", 15.f, L"Exit");

    {
        this->testRect = rendererFactory.MakeRectangleRenderer();

        GameRenderer::FilledRectangleGeometryParams rectGeom;

        rectGeom.color.r = 0;
        rectGeom.color.g = 255;
        rectGeom.color.b = 0;

        rectGeom.width = 1.5f;

        rectGeom.roundness = this->rectRoundnessAngle;

        this->testRect->SetGeometryParams(rectGeom);
    }

    {
        float startY = 0.f;
        float stepY = 0.02f;
        float height = 0.00001f;
        int lineCount = 40;

        for (int i = 0; i < lineCount; i++) {
            auto rectRender = rendererFactory.MakeRectangleRenderer();
            auto rectRender2 = rendererFactory.MakeRectangleRenderer();

            GameRenderer::FilledRectangleGeometryParams rectGeom;

            rectGeom.height = height;
            rectGeom.color.r = 0;
            rectGeom.color.g = 255;
            rectGeom.color.b = 0;

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
        this->shipLivesStateRect = rendererFactory.MakeRectangleRenderer();

        GameRenderer::HollowRectangleGeometryParams rectGeom;

        rectGeom.width = SpaceGameRenderer::ShipLivesStateRectWidth;
        rectGeom.height = SpaceGameRenderer::ShipLivesStateRectHeight;
        rectGeom.thickness = 0.005f;
        rectGeom.color.r = 0;
        rectGeom.color.g = 255;
        rectGeom.color.b = 0;

        this->shipLivesStateRect->SetGeometryParams(rectGeom);
    }

    {
        {
            this->shipLivesMainBg = rendererFactory.MakeRectangleRenderer();

            GameRenderer::FilledRectangleGeometryParams rectGeom;

            rectGeom.width = SpaceGameRenderer::ShipLivesMainWidth;
            rectGeom.height = SpaceGameRenderer::ShipLivesMainHeight;
            rectGeom.color.r = 0;
            rectGeom.color.g = 0;
            rectGeom.color.b = 0;

            this->shipLivesMainBg->SetGeometryParams(rectGeom);
        }

        {
            this->shipLivesMainFrame = rendererFactory.MakeRectangleRenderer();

            GameRenderer::HollowRectangleGeometryParams rectGeom;

            rectGeom.width = SpaceGameRenderer::ShipLivesMainWidth;
            rectGeom.height = SpaceGameRenderer::ShipLivesMainHeight;
            rectGeom.thickness = 0.005f;
            rectGeom.color.r = 0;
            rectGeom.color.g = 0;
            rectGeom.color.b = 255;

            this->shipLivesMainFrame->SetGeometryParams(rectGeom);
        }
    }

    {
        {
            this->gameFieldMainBg = rendererFactory.MakeRectangleRenderer();

            GameRenderer::FilledRectangleGeometryParams rectGeom;

            rectGeom.width = SpaceGameRenderer::GameFieldMainWidth;
            rectGeom.height = SpaceGameRenderer::GameFieldMainHeight;
            rectGeom.color.r = 0;
            rectGeom.color.g = 0;
            rectGeom.color.b = 0;

            this->gameFieldMainBg->SetGeometryParams(rectGeom);
        }

        {
            this->gameFieldMainFrame = rendererFactory.MakeRectangleRenderer();

            GameRenderer::HollowRectangleGeometryParams rectGeom;

            rectGeom.width = SpaceGameRenderer::GameFieldMainWidth;
            rectGeom.height = SpaceGameRenderer::GameFieldMainHeight;
            rectGeom.thickness = 0.005f;
            rectGeom.color.r = 0;
            rectGeom.color.g = 0;
            rectGeom.color.b = 255;

            this->gameFieldMainFrame->SetGeometryParams(rectGeom);
        }

        {
            RGBA8Color color[2] = {};

            color[0].a = 0;
            color[0].r = 255;
            color[0].g = 255;
            color[0].b = 255;

            color[1].a = 255;
            color[1].r = 255;
            color[1].g = 255;
            color[1].b = 255;

            this->testTex = rendererFactory.MakeTexture2DFromMemory(1, 2, GameRenderer::TexturePixelFormat::BGRA8, &color, sizeof(color));
        }

        {
            const size_t zoneCount = 3;
            const float stepX = SpaceGameRenderer::GameFieldMainWidth / (zoneCount );
            const float zoneLineHeight = 0.7f;
            const float zoneLineY = (SpaceGameRenderer::GameFieldMainHeight * 0.5f) - (zoneLineHeight * 0.5f);

            for (size_t i = 1; i < zoneCount; ++i) {
                float x = i * stepX + (-SpaceGameRenderer::GameFieldMainWidth * 0.5f);

                GameRenderer::FilledRectangleGeometryParams rectGeom;

                rectGeom.width = 0.002f;
                rectGeom.height = zoneLineHeight;
                rectGeom.color.r = 255;
                rectGeom.color.g = 0;
                rectGeom.color.b = 0;

                auto line = rendererFactory.MakeRectangleRenderer();

                line->SetGeometryParams(rectGeom);

                auto transform = line->GetRectangleTransform();

                transform.position.x = x;
                transform.position.y = zoneLineY;

                line->SetRectangleTransform(transform);

                this->gameFieldEnemyZoneLines.push_back(std::move(line));
            }
        }
    }

    this->stars = std::make_unique<Stars>(0.15f, 30, Math::Vector2{ SpaceGameRenderer::GameFieldMainWidth, SpaceGameRenderer::GameFieldMainHeight }, rendererFactory);
}

SpaceGameRenderer::~SpaceGameRenderer() {
}

void SpaceGameRenderer::Render() {
    //TextRenderer text(L"TEST");

    //auto ctxLk = this->dxDev->LockCtxScoped();

    //{
    //    /*Filesystem::StreamFILE stream(LR"(Fonts\\NotoSans-Regular.ttf)", Filesystem::StreamFILEMode::ReadOnly);
    //    auto fontRenderer = std::make_unique<FreetypeFontRenderer>(stream);
    //    auto fntAtlas = std::make_unique<SimpleDxFontAtlas>(this->dxDev->GetD3DDevice(), this->dxDev->D3D(), std::move(fontRenderer));

    //    auto symbol = fntAtlas->RenderSymbol(10.1f, 'A');*/
    //}

    this->Update();

    auto opScope = this->renderer->OperationBeginScoped();

    auto rectTransform = this->testRect->GetRectangleTransform();
    rectTransform.rotation.x += DirectX::XMConvertToRadians(0.4f);
    rectTransform.rotation.z = DirectX::XMConvertToRadians(30.f);
    this->testRect->SetRectangleTransform(rectTransform);

    auto geomParams = std::get<GameRenderer::FilledRectangleGeometryParams>(this->testRect->GetGeometryParams());
    geomParams.roundness = std::fabs(std::sin(this->rectRoundnessAngle));
    this->testRect->SetGeometryParams(geomParams);

    auto alphaBlendScope = this->renderer->PushAlphaBlendingScoped(true);

    this->renderer->RenderBackgroundBrush(this->bgBrush);
    //this->renderer->RenderRectangle(this->testRect);

    // shipLives
    //if(false)
    {
        auto matrixScope = this->renderer->PushWorldMatrixScoped(Math::Matrix4::Translate(-(SpaceGameRenderer::GameFieldMainWidth * 0.5f) - (SpaceGameRenderer::ShipLivesMainWidth * 0.5f), 0.1f));

        const Math::Vector2 scissorScale = { SpaceGameRenderer::ShipLivesStateRectWidth, SpaceGameRenderer::ShipLivesStateRectHeight };
        const Math::Vector2 scissorPos = { 0.f, 0.f };

        this->renderer->RenderRectangle(this->shipLivesMainBg);

        {
            auto scissorScope = this->renderer->PushScissorScoped(scissorPos, scissorScale);

            for (auto& i : this->shipLivesBgCrossHatchFill) {
                this->renderer->RenderRectangle(i);
            }
        }

        this->renderer->RenderRectangle(this->shipLivesStateRect);

        this->renderer->RenderRectangle(this->shipLivesMainFrame);
    }

    // gameField
    {
        this->renderer->RenderRectangle(this->gameFieldMainBg);

        this->stars->Draw(*this->renderer);

        for (const auto& zoneLine : this->gameFieldEnemyZoneLines) {
            this->renderer->RenderRectangle(zoneLine, this->testTex);
        }

        this->renderer->RenderRectangle(this->gameFieldMainFrame);
    }

    this->renderer->RenderText(this->testText);

    this->rectRoundnessAngle += 0.01f;
}

void SpaceGameRenderer::OutputParametersChanged() {
    //auto ctxLk = this->dxDev->LockCtxScoped();
}

void SpaceGameRenderer::Update() {
    float dt = 0.f;

    auto curTime = std::chrono::high_resolution_clock::now();

    if (this->prevTime) {
        auto dtDuration = curTime - *this->prevTime;
        auto dtSeconds = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(dtDuration);

        dt = dtSeconds.count();

        if (dt > 1.f) {
            dt = 1.f;
        }
    }

    this->prevTime = curTime;

    this->stars->Update(dt);
}
