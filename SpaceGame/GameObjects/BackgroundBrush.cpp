#include "BackgroundBrush.h"
#include "..\Geometry\TexVertex.h"

#include <libhelpers\HSystem.h>
#include <libhelpers\ImageUtils.h>

BackgroundBrush::BackgroundBrush(DxDevice *dxDev, const std::wstring &texName) {
    HRESULT hr = S_OK;
    auto d3dDev = dxDev->GetD3DDevice();
    auto pkgPath = H::System::GetPackagePath();

    // Texture
    {
        ImageUtils img;
        auto dec = img.CreateDecoder(pkgPath + texName);
        auto pixels = ImageUtils::LoadDxPBgra32Image(dec, 2048);
        Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;

        D3D11_TEXTURE2D_DESC tex2dDesc;
        D3D11_SUBRESOURCE_DATA initData;

        tex2dDesc.Width = pixels.Size.x;
        tex2dDesc.Height = pixels.Size.y;
        tex2dDesc.MipLevels = 1;
        tex2dDesc.ArraySize = 1;
        tex2dDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        tex2dDesc.SampleDesc.Count = 1;
        tex2dDesc.SampleDesc.Quality = 0;
        tex2dDesc.Usage = D3D11_USAGE_IMMUTABLE;
        tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex2dDesc.CPUAccessFlags = 0;
        tex2dDesc.MiscFlags = 0;

        initData.pSysMem = pixels.Data.data();
        initData.SysMemPitch = pixels.Size.x * 4;
        initData.SysMemSlicePitch = initData.SysMemPitch * pixels.Size.y;

        hr = d3dDev->CreateTexture2D(&tex2dDesc, &initData, tex2d.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        hr = d3dDev->CreateShaderResourceView(tex2d.Get(), nullptr, this->srv.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    // geometry
    {
        TexVertex2D vertices[] = {
            { { -0.5f, 0.5f }, { 0.0f, 0.0f } },
            { { 0.5f, 0.5f }, { 1.0f, 0.0f } },
            { { 0.5f, -0.5f }, { 1.0f, 1.0f } },
            { { -0.5f, -0.5f }, { 0.0f, 1.0f } }
        };

        D3D11_BUFFER_DESC bufDesc;
        D3D11_SUBRESOURCE_DATA initData;

        bufDesc.ByteWidth = sizeof(vertices);
        bufDesc.Usage = D3D11_USAGE_DEFAULT;
        bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufDesc.CPUAccessFlags = 0;
        bufDesc.MiscFlags = 0;
        bufDesc.StructureByteStride = 0;

        initData.pSysMem = vertices;
        initData.SysMemPitch = initData.SysMemSlicePitch = 0;

        hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->vertexBuf.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    // vertex shader and input layout
    {
        auto data = H::System::LoadPackageFile(L"TexVs.cso");

        hr = d3dDev->CreateVertexShader(data.data(), data.size(), nullptr, this->vs.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        D3D11_BUFFER_DESC bufDesc;

        bufDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
        bufDesc.Usage = D3D11_USAGE_DEFAULT;
        bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufDesc.CPUAccessFlags = 0;
        bufDesc.MiscFlags = 0;
        bufDesc.StructureByteStride = 0;

        hr = d3dDev->CreateBuffer(&bufDesc, nullptr, this->vsCBuf.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        hr = d3dDev->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), data.data(), data.size(), this->inputLayout.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    // pixel shader
    {
        auto data = H::System::LoadPackageFile(L"TexPs.cso");

        hr = d3dDev->CreatePixelShader(data.data(), data.size(), nullptr, this->ps.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    // sampler state
    {
        D3D11_SAMPLER_DESC desc;

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // D3D11_TEXTURE_ADDRESS_MIRROR; //
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        std::memcpy(desc.BorderColor, DirectX::g_XMZero, sizeof(float) * 4);

        hr = d3dDev->CreateSamplerState(&desc, this->samplerState.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }
}