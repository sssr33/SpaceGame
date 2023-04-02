#pragma once
#include "../IGameRenderer.h"
#include "GameRendererFactoryDx.h"

#include <libhelpers/Dx/Dx.h>
#include <libhelpers/Dx/Renderer/IOutput.h>
#include <stack>

namespace GameRenderer {
    class GameRendererDx : public IGameRenderer {
    public:
        GameRendererDx(DxDevice* dxDev, IOutput* output);

        IGameRendererFactory& GetFactory() override;

        void OperationBegin() override;
        void OperationEnd() override;

        void PushAlphaBlending(bool premultiplied) override;
        void PopAlphaBlending() override;

    private:
        struct BlendState {
            Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
            std::array<float, 4> blendFactor = { 1.f, 1.f, 1.f, 1.f };
            UINT sampleMask = 0xFFFFFFu;

            BlendState() = default;
            BlendState(ID3D11Device* dev, D3D11_BLEND srcBlend, D3D11_BLEND destBlend, D3D11_BLEND srcBlendAlpha, D3D11_BLEND destBlendAlpha);

            void Set(ID3D11DeviceContext* d3d) const;
        };

        void DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) override;
        void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) override;
        void DoRenderText(const std::shared_ptr<ITextRenderer>& obj) override;

        void PushAlphaBlendState();
        void PushAlphaBlendStatePremultiplied();

        DxDevice* dxDev = nullptr;
        IOutput* output = nullptr;

        GameRendererFactoryDx factory;

        std::stack<BlendState> blendStateStack;
        BlendState alphaBlend;
        BlendState alphaBlendPremultiplied;
    };
}
