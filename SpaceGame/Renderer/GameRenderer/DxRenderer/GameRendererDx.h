#pragma once
#include "../IGameRenderer.h"
#include "GameRendererFactoryDx.h"

#include <libhelpers/Dx/Dx.h>
#include <libhelpers/Dx/Renderer/IOutput.h>
#include <libhelpers/AlignedAllocator.h>
#include <stack>

namespace GameRenderer {
    class GameRendererDx final : public IGameRenderer {
    public:
        GameRendererDx(DxDevice* dxDev, IOutput* output);

        IGameRendererFactory& GetFactory() override;

        Math::Vector2 GetScreenPixelSize() override;

        void OperationBegin() override;
        void OperationEnd() override;

        void PushAlphaBlending(bool premultiplied) override;
        void PopAlphaBlending() override;

        void PushScissor(const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize) override;
        void PopScissor() override;

        void PushWorldMatrix(const Math::Matrix4& m) override;
        void PushWorldMatrixAdditive(const Math::Matrix4& m) override;
        void PopWorldMatrix() override;

        Math::Vector3 GetWorldCoordsFromPixel(const Math::Vector2& pos) override;

    private:
        struct BlendState {
            Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
            std::array<float, 4> blendFactor = { 1.f, 1.f, 1.f, 1.f };
            UINT sampleMask = 0xFFFFFFu;

            BlendState() = default;
            BlendState(ID3D11Device* dev, D3D11_BLEND srcBlend, D3D11_BLEND destBlend, D3D11_BLEND srcBlendAlpha, D3D11_BLEND destBlendAlpha);

            void Set(ID3D11DeviceContext* d3d) const;
        };

        struct ScissorState {
            Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState;
            std::vector<D3D11_RECT> rects;

            void Set(ID3D11DeviceContext* d3d) const;
        };

        void DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) override;
        void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) override;
        void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj, const std::shared_ptr<ITexture2D>& tex) override;
        void DoRenderText(const std::shared_ptr<ITextRenderer>& obj) override;

        void PushAlphaBlendState();
        void PushAlphaBlendStatePremultiplied();

        D3D11_RECT TransformScissorToScreen(ID3D11DeviceContext* d3d, const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize);

        DirectX::XMMATRIX GetCurrentWorldMatrix() const;

        static DirectX::XMMATRIX ToXMMatrix(const Math::Matrix4& m);

        DxDevice* dxDev = nullptr;
        IOutput* output = nullptr;

        GameRendererFactoryDx factory;

        std::stack<BlendState> blendStateStack;
        BlendState alphaBlend;
        BlendState alphaBlendPremultiplied;

        std::stack<ScissorState> scissorStateStack;

        std::stack<DirectX::XMMATRIX, std::deque<DirectX::XMMATRIX, aligned_allocator<DirectX::XMMATRIX, 16>>> worldMatrixStack;
    };
}
