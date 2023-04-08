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

    Math::Vector2 GameRendererDx::GetScreenPixelSize() {
        uint32_t numViewPorts = 1;
        D3D11_VIEWPORT viewport;
        auto d3dCtx = dxDev->D3D();

        d3dCtx->RSGetViewports(&numViewPorts, &viewport);

        return { viewport.Width, viewport.Height };
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

        if (premultiplied) {
            this->PushAlphaBlendStatePremultiplied();
        }
        else {
            this->PushAlphaBlendState();
        }

        // push to stack after new state create for exception safety
        this->blendStateStack.push(state);
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

    void GameRendererDx::PushScissor(const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize) {
        auto d3d = this->dxDev->D3D();

        ScissorState state;
        D3D11_RASTERIZER_DESC rsDesc = {};

        d3d->RSGetState(state.rasterState.GetAddressOf());

        if (state.rasterState) {
            state.rasterState->GetDesc(&rsDesc);
        }
        else {
            rsDesc.FillMode = D3D11_FILL_SOLID;
            rsDesc.CullMode = D3D11_CULL_BACK;
            rsDesc.DepthClipEnable = TRUE;
        }

        UINT numRects = 0;
        d3d->RSGetScissorRects(&numRects, nullptr);
        state.rects.resize(numRects);
        d3d->RSGetScissorRects(&numRects, state.rects.data());

        rsDesc.ScissorEnable = TRUE;

        HRESULT hr = S_OK;
        auto dev = this->dxDev->GetD3DDevice();
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> newRsState;
        D3D11_RECT scissorRect = this->TransformScissorToScreen(d3d, scissorCenter, scissorHalfSize);

        hr = dev->CreateRasterizerState(&rsDesc, newRsState.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        d3d->RSSetState(newRsState.Get());
        d3d->RSSetScissorRects(1, &scissorRect);

        // push to stack after new state create for exception safety
        this->scissorStateStack.push(state);
    }

    void GameRendererDx::PopScissor() {
        if (this->scissorStateStack.empty()) {
            assert(false);
            return;
        }

        auto d3d = this->dxDev->D3D();

        const auto& state = this->scissorStateStack.top();
        state.Set(d3d);
        this->scissorStateStack.pop();
    }

    void GameRendererDx::PushWorldMatrix(const Math::Matrix4& m) {
        this->worldMatrixStack.push(GameRendererDx::ToXMMatrix(m));
    }

    void GameRendererDx::PushWorldMatrixAdditive(const Math::Matrix4& m) {
        auto newMatrix = GameRendererDx::ToXMMatrix(m);

        if (!this->worldMatrixStack.empty()) {
            const auto& curMatrix = this->worldMatrixStack.top();
            newMatrix = DirectX::XMMatrixMultiply(curMatrix, newMatrix);
        }

        this->worldMatrixStack.push(newMatrix);
    }

    void GameRendererDx::PopWorldMatrix() {
        this->worldMatrixStack.pop();
    }

    void GameRendererDx::DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) {
        BackgroundBrushRendererDx& devObj = static_cast<BackgroundBrushRendererDx&>(*obj);
        devObj.Render(this->dxDev);
    }

    void GameRendererDx::DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) {
        RectangleRendererDx& devObj = static_cast<RectangleRendererDx&>(*obj);
        devObj.Render(this->dxDev, this->GetCurrentWorldMatrix());
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

    D3D11_RECT GameRendererDx::TransformScissorToScreen(ID3D11DeviceContext* d3d, const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize) {
        const auto screenPixSize = this->GetScreenPixelSize();
        const float ar = screenPixSize.x / screenPixSize.y;

        auto transform = DirectX::XMMatrixScaling(scissorHalfSize.x, scissorHalfSize.y, 1.f);
        transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixTranslation(scissorCenter.x, scissorCenter.y, 1.f));
        transform = DirectX::XMMatrixMultiply(transform, this->GetCurrentWorldMatrix());
        transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.f), ar, 0.001f, 10.0f));

        auto leftTop = DirectX::XMVectorSet(-0.5f, 0.5f, 0.f, 1.f);
        auto rightBottom = DirectX::XMVectorSet(0.5f, -0.5f, 0.f, 1.f);

        leftTop = DirectX::XMVector3Project(leftTop, 0.f, 0.f, screenPixSize.x, screenPixSize.y, 0.f, 1.f, transform, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());
        rightBottom = DirectX::XMVector3Project(rightBottom, 0.f, 0.f, screenPixSize.x, screenPixSize.y, 0.f, 1.f, transform, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());

        D3D11_RECT box;

        box.left = static_cast<int>(DirectX::XMVectorGetX(leftTop));
        box.top = static_cast<int>(DirectX::XMVectorGetY(leftTop));
        box.right = static_cast<int>(DirectX::XMVectorGetX(rightBottom));
        box.bottom = static_cast<int>(DirectX::XMVectorGetY(rightBottom));

        return box;
    }

    DirectX::XMMATRIX GameRendererDx::GetCurrentWorldMatrix() const {
        if (this->worldMatrixStack.empty()) {
            return DirectX::XMMatrixIdentity();
        }

        return this->worldMatrixStack.top();
    }

    DirectX::XMMATRIX GameRendererDx::ToXMMatrix(const Math::Matrix4& m) {
        return DirectX::XMMatrixSet(
            m._00, m._01, m._02, m._03,
            m._10, m._11, m._12, m._13,
            m._20, m._21, m._22, m._23,
            m._30, m._31, m._32, m._33
            );
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




    void GameRendererDx::ScissorState::Set(ID3D11DeviceContext* d3d) const {
        d3d->RSSetState(this->rasterState.Get());
        d3d->RSSetScissorRects(static_cast<UINT>(this->rects.size()), this->rects.data());
    }
}
