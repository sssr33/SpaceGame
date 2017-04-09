#pragma once

#include <libhelpers\Dx\Dx.h>

class BackgroundBrush {
public:
    BackgroundBrush(DxDevice *dxDev, const std::wstring &texName);

    void Render(DxDevice *dxDev);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuf;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuf;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;

    DirectX::XMFLOAT2 lastTargetSize;
    DirectX::XMFLOAT2 texSize;
};