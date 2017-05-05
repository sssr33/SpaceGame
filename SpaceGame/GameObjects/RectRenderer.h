#pragma once

#include <libhelpers\Dx\Dx.h>

class RectRenderer {
public:
    RectRenderer(DxDevice *dxDev);

    void Render(DxDevice *dxDev);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuf;
    Microsoft::WRL::ComPtr<ID3D11Buffer> idxBuf;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuf;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;

    /*Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dsState;*/
};