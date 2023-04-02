#include "GameRendererDx.h"
#include "BackgroundBrushRendererDx.h"
#include "RectangleRendererDx.h"
#include "TextRendererDx.h"

#include <libhelpers/HashString.h>
#include <libhelpers/HSystem.h>

namespace GameRenderer {
    GameRendererDx::GameRendererDx(DxDevice* dxDev, IOutput* output)
        : IGameRenderer(HashString<uint32_t>("GameRendererDx"))
        , dxDev(dxDev)
        , output(output)
        , factory(this->GetRendererId(), dxDev, output)
    {}

    IGameRendererFactory& GameRendererDx::GetFactory() {
        return this->factory;
    }

    void GameRendererDx::OperationBegin() {
        this->dxDev->LockCtx();
    }

    void GameRendererDx::OperationEnd() {
        this->dxDev->UnlockCtx();
    }

    void GameRendererDx::PushAlphaBlending(bool premultiplied) {
        auto d3d = this->dxDev->D3D();

        BlendState state;
        d3d->OMGetBlendState(state.blendState.GetAddressOf(), state.blendFactor.data(), &state.sampleMask);
        this->blendStateStack.push(state);

        if (premultiplied) {
            this->PushAlphaBlendStatePremultiplied();
        }
        else {
            this->PushAlphaBlendState();
        }
    }

    void GameRendererDx::PopAlphaBlending() {
        if (this->blendStateStack.empty()) {
            assert(false);
            return;
        }

        auto d3d = this->dxDev->D3D();

        const auto& state = this->blendStateStack.top();
        state.Set(d3d);
        this->blendStateStack.pop();
    }

    void GameRendererDx::DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) {
        BackgroundBrushRendererDx& devObj = static_cast<BackgroundBrushRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }

    void GameRendererDx::DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) {
        RectangleRendererDx& devObj = static_cast<RectangleRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }

    void GameRendererDx::DoRenderText(const std::shared_ptr<ITextRenderer>& obj) {
        TextRendererDx& devObj = static_cast<TextRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }

    void GameRendererDx::PushAlphaBlendState() {
        if (!this->alphaBlend.blendState) {
            auto dev = this->dxDev->GetD3DDevice();
            this->alphaBlend = BlendState(dev, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
        }

        auto d3d = this->dxDev->D3D();
        this->alphaBlend.Set(d3d);
    }

    void GameRendererDx::PushAlphaBlendStatePremultiplied() {
        if (!this->alphaBlendPremultiplied.blendState) {
            auto dev = this->dxDev->GetD3DDevice();
            this->alphaBlendPremultiplied = BlendState(dev, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);
        }

        auto d3d = this->dxDev->D3D();
        this->alphaBlendPremultiplied.Set(d3d);
    }




    GameRendererDx::BlendState::BlendState(ID3D11Device* dev, D3D11_BLEND srcBlend, D3D11_BLEND destBlend, D3D11_BLEND srcBlendAlpha, D3D11_BLEND destBlendAlpha) {
        HRESULT hr = S_OK;

        D3D11_BLEND_DESC blendDesc = {};
        auto& rtDesc = blendDesc.RenderTarget[0];

        rtDesc.BlendEnable = TRUE;

        rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtDesc.DestBlend = destBlend;
        rtDesc.SrcBlend = srcBlend;

        rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        rtDesc.DestBlendAlpha = destBlendAlpha;
        rtDesc.SrcBlendAlpha = srcBlendAlpha;

        rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = dev->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    void GameRendererDx::BlendState::Set(ID3D11DeviceContext* d3d) const {
        d3d->OMSetBlendState(this->blendState.Get(), this->blendFactor.data(), this->sampleMask);
    }
}
