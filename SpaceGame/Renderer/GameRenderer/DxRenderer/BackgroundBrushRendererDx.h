#pragma once
#include "../IBackgroundBrushRenderer.h"

#include <libhelpers\Dx\Dx.h>

namespace GameRenderer {
    class BackgroundBrushRendererDx : public IBackgroundBrushRenderer {
    public:
        BackgroundBrushRendererDx(uint32_t rendererId, DxDevice* dxDev, const std::wstring& texName);

        void Render(DxDevice* dxDev);

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuf;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
        Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuf;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;

        DirectX::XMFLOAT2 lastTargetSize = { 0.f, 0.f };
        DirectX::XMFLOAT2 texSize = { 0.f, 0.f };
    };
}
