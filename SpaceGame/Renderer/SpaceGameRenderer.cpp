#include "SpaceGameRenderer.h"

SpaceGameRenderer::SpaceGameRenderer(raw_ptr<DxDevice> dxDev, raw_ptr<IOutput> output)
    : IRenderer(dxDev, output)
{}

SpaceGameRenderer::~SpaceGameRenderer() {
}

void SpaceGameRenderer::Render() {
}

void SpaceGameRenderer::OutputParametersChanged() {
}