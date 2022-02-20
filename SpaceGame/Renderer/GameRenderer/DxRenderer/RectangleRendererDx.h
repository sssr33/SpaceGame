#pragma once
#include "../IRectangleRenderer.h"

#include <libhelpers\Dx\Dx.h>

namespace GameRenderer {
    class RectangleRendererDx : public IRectangleRenderer {
    public:
        RectangleRendererDx(uint32_t rendererId, DxDevice* dxDev);

        void Render(DxDevice* dxDev);

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuf;
        Microsoft::WRL::ComPtr<ID3D11Buffer> idxBuf;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
        Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuf;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;

        uint32_t indexCount;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;
        /*Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dsState;*/
    };
}
