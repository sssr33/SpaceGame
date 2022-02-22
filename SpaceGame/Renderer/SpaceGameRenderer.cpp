#include "SpaceGameRenderer.h"

#include <libhelpersDesktop/Filesystem/StreamFILE.h>

SpaceGameRenderer::SpaceGameRenderer(std::shared_ptr<GameRenderer::IGameRenderer> renderer)
    : renderer(std::move(renderer))
{
    auto opScope = this->renderer->OperationBeginScoped();
    auto& rendererFactory = this->renderer->GetFactory();

    this->bgBrush = rendererFactory.MakeBackgroundBrushRenderer(L"Assets\\back.bmp");
    this->testRect = rendererFactory.MakeRectangleRenderer();
    this->testText = rendererFactory.MakeTextRenderer(L"Arial", 15.f, L"Exit");

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

    this->renderer->RenderBackgroundBrush(this->bgBrush);
    this->renderer->RenderRectangle(this->testRect);
    this->renderer->RenderText(this->testText);
}

void SpaceGameRenderer::OutputParametersChanged() {
    //auto ctxLk = this->dxDev->LockCtxScoped();
}