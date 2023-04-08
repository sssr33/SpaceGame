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

            this->bgCrossHatchFill.push_back(std::move(rectRender));
            this->bgCrossHatchFill.push_back(std::move(rectRender2));
        }
    }

    {
        this->shipLivesStateRect = rendererFactory.MakeRectangleRenderer();

        GameRenderer::HollowRectangleGeometryParams rectGeom;

        rectGeom.width = 0.16f;
        rectGeom.height = 0.16f;
        rectGeom.thickness = 0.005f;
        rectGeom.color.r = 0;
        rectGeom.color.g = 255;
        rectGeom.color.b = 0;

        this->shipLivesStateRect->SetGeometryParams(rectGeom);
    }
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

    {
        auto matrixScope = this->renderer->PushWorldMatrixScoped(Math::Matrix4::Translate(0.f, 0.4f));

        const Math::Vector2 scissorScale = { 0.16f, 0.16f };
        const Math::Vector2 scissorPos = { 0.f, 0.f };

        {
            auto scissorScope = this->renderer->PushScissorScoped(scissorPos, scissorScale);

            for (auto& i : this->bgCrossHatchFill) {
                this->renderer->RenderRectangle(i);
            }
        }

        this->renderer->RenderRectangle(this->shipLivesStateRect);
    }

    this->renderer->RenderText(this->testText);

    this->rectRoundnessAngle += 0.01f;
}

void SpaceGameRenderer::OutputParametersChanged() {
    //auto ctxLk = this->dxDev->LockCtxScoped();
}