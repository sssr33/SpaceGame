#include "SpaceGameRenderer.h"
//#include "..\Text\TextRenderer.h"
#include "../Text/Freetype/FreetypeFontRenderer.h"
#include "../Text/SimpleDxFontAtlas/SimpleDxFontAtlas.h"

#include <libhelpersDesktop/Filesystem/StreamFILE.h>

SpaceGameRenderer::SpaceGameRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
    : IRenderer(dxDev, output), bgBrush(dxDev.get(), L"Assets\\back.bmp"),
    testRect(dxDev.get())
{
}

SpaceGameRenderer::~SpaceGameRenderer() {
}

void SpaceGameRenderer::Render() {
    //TextRenderer text(L"TEST");

    auto ctxLk = this->dxDev->LockCtxScoped();

    {
        Filesystem::StreamFILE stream(LR"(Fonts\\NotoSans-Regular.ttf)", Filesystem::StreamFILEMode::ReadOnly);
        auto fontRenderer = std::make_unique<FreetypeFontRenderer>(stream);
        auto fntAtlas = std::make_unique<SimpleDxFontAtlas>(this->dxDev->GetD3DDevice(), this->dxDev->D3D(), std::move(fontRenderer));

        auto symbol = fntAtlas->RenderSymbol(10.1f, 'A');
    }

    this->bgBrush.Render(this->dxDev.get());
    this->testRect.Render(this->dxDev.get());
}

void SpaceGameRenderer::OutputParametersChanged() {
    auto ctxLk = this->dxDev->LockCtxScoped();
}