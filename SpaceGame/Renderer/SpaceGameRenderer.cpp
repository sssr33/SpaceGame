#include "SpaceGameRenderer.h"

SpaceGameRenderer::SpaceGameRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
    : IRenderer(dxDev, output), bgBrush(dxDev.get(), L"Assets\\back.bmp")
{}

SpaceGameRenderer::~SpaceGameRenderer() {
}

void SpaceGameRenderer::Render() {
    auto ctxLk = this->dxDev->LockCtxScoped();

    this->bgBrush.Render(this->dxDev.get());
}

void SpaceGameRenderer::OutputParametersChanged() {
    auto ctxLk = this->dxDev->LockCtxScoped();
}