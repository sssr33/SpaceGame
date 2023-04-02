#include "SpaceGameRenderer.h"

#include <libhelpersDesktop/Filesystem/StreamFILE.h>
#include <cmath>

SpaceGameRenderer::SpaceGameRenderer(std::shared_ptr<GameRenderer::IGameRenderer> renderer)
    : renderer(std::move(renderer))
{
    auto opScope = this->renderer->OperationBeginScoped();
    auto& rendererFactory = this->renderer->GetFactory();

    this->bgBrush = rendererFactory.MakeBackgroundBrushRenderer(L"Assets\\back.bmp");
    this->testRect = rendererFactory.MakeRectangleRenderer();
    this->testText = rendererFactory.MakeTextRenderer(L"Arial", 15.f, L"Exit");

    GameRenderer::FilledRectangleGeometryParams rectGeom;

    rectGeom.color.r = 0;
    rectGeom.color.g = 255;
    rectGeom.color.b = 0;

    rectGeom.width = 1.5f;

    rectGeom.roundness = this->rectRoundnessAngle;

    this->testRect->SetGeometryParams(rectGeom);


    float startY = 0.f;
    float stepY = 0.02f;
    float height = 0.00001f;

    for (int i = 0; i < 40; i++) {
        auto rectRender = rendererFactory.MakeRectangleRenderer();
        auto rectRender2 = rendererFactory.MakeRectangleRenderer();

        GameRenderer::FilledRectangleGeometryParams rectGeom2;

        rectGeom2.height = height;

        auto transform = rectRender->GetRectangleTransform();

        transform.position.y = i * stepY;
        transform.rotation.z = DirectX::XMConvertToRadians(45.f);

        rectRender->SetGeometryParams(rectGeom2);
        rectRender->SetRectangleTransform(transform);

        transform.rotation.z = DirectX::XMConvertToRadians(-45.f);

        rectRender2->SetGeometryParams(rectGeom2);
        rectRender2->SetRectangleTransform(transform);

        this->bgCrossHatchFill.push_back(std::move(rectRender));
        this->bgCrossHatchFill.push_back(std::move(rectRender2));
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

    for (auto& i : this->bgCrossHatchFill) {
        this->renderer->RenderRectangle(i);
    }

    this->renderer->RenderText(this->testText);

    this->rectRoundnessAngle += 0.01f;
}

void SpaceGameRenderer::OutputParametersChanged() {
    //auto ctxLk = this->dxDev->LockCtxScoped();
}