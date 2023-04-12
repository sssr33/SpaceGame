#pragma once
#include "../IRectangleRenderer.h"
#include "Texture2DDx.h"

#include <libhelpers/Dx/Dx.h>

namespace GameRenderer {
    class RectangleRendererDx : public IRectangleRenderer {
    public:
        RectangleRendererDx(uint32_t rendererId, DxDevice* dxDev);

        GeometryParams GetGeometryParams() override;
        void SetGeometryParams(const GeometryParams& params) override;

        RectangleTransform GetRectangleTransform() override;
        void SetRectangleTransform(const RectangleTransform& transform) override;

        void Render(DxDevice* dxDev, const DirectX::XMMATRIX& worldTransform);
        void Render(DxDevice* dxDev, Texture2DDx& tex, const DirectX::XMMATRIX& worldTransform);

    private:
        void CheckGeometry(DxDevice* dxDev);
        void CheckTransform(DxDevice* dxDev, const DirectX::XMMATRIX& worldTransform);

        GeometryParams geomParams;
        bool geomParamsUpdated = true;

        RectangleTransform transform;
        bool transformUpdated = true;
        float lastScreenWidth = 0.f;
        float lastScreenHeight = 0.f;
        DirectX::XMFLOAT4X4 prevWorldTransform;

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuf;
        Microsoft::WRL::ComPtr<ID3D11Buffer> idxBuf;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
        Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuf;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> psTex;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

        uint32_t indexCount = 0;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;
        /*Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dsState;*/
    };
}
