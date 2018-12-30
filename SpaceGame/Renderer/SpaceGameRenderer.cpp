#include "SpaceGameRenderer.h"
#include "..\Text\TextRenderer.h"

SpaceGameRenderer::SpaceGameRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
    : IRenderer(dxDev, output), bgBrush(dxDev.get(), L"Assets\\back.bmp"),
    testRect(dxDev.get())
{
}

SpaceGameRenderer::~SpaceGameRenderer() {
}

void SpaceGameRenderer::Render() {
    TextRenderer text(L"TEST");

    auto ctxLk = this->dxDev->LockCtxScoped();

    this->bgBrush.Render(this->dxDev.get());
    this->testRect.Render(this->dxDev.get());
}

void SpaceGameRenderer::OutputParametersChanged() {
    auto ctxLk = this->dxDev->LockCtxScoped();
}