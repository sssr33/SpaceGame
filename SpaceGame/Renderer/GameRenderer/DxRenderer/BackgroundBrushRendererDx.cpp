#include "BackgroundBrushRendererDx.h"
#include "Geometry\TexVertex.h"

#include <libhelpers\HSystem.h>
#include <libhelpers\ImageUtils.h>

namespace GameRenderer {
    BackgroundBrushRendererDx::BackgroundBrushRendererDx(uint32_t rendererId, DxDevice* dxDev, const std::wstring& texName)
        : IBackgroundBrushRenderer(rendererId)
    {
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

            this->texSize.x = (float)pixels.Size.x;
            this->texSize.y = (float)pixels.Size.y;
        }

        // geometry
        {
            TexVertex2D vertices[] = {
                { { -0.5f, 0.5f }, { 0.0f, 0.0f } },
                { { 0.5f, 0.5f }, { 1.0f, 0.0f } },
                { { -0.5f, -0.5f },{ 0.0f, 1.0f } },
                { { 0.5f, -0.5f }, { 1.0f, 1.0f } },
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

    void BackgroundBrushRendererDx::Render(DxDevice* dxDev) {
        auto d3dCtx = dxDev->D3D();

        uint32_t numViewPorts = 1;
        D3D11_VIEWPORT viewport;

        d3dCtx->RSGetViewports(&numViewPorts, &viewport);

        if (this->lastTargetSize.x != viewport.Width || this->lastTargetSize.y != viewport.Height) {
            this->lastTargetSize.x = viewport.Width;
            this->lastTargetSize.y = viewport.Height;

            DirectX::XMFLOAT2 texLT, texRB;

            if (this->lastTargetSize.x == 0 || this->lastTargetSize.y == 0) {
                texLT.x = texLT.y = 0.0f;
                texRB.x = texRB.y = 1.0f;
            }
            else {
                DirectX::XMFLOAT2 texCoordSize;

                texCoordSize.x = this->lastTargetSize.x / this->texSize.x;
                texCoordSize.y = this->lastTargetSize.y / this->texSize.y;

                texLT.x = -texCoordSize.x * 0.5f;
                texLT.y = -texCoordSize.y * 0.5f;
                texRB.x = texCoordSize.x * 0.5f;
                texRB.y = texCoordSize.y * 0.5f;
            }

            TexVertex2D vertices[] = {
                { { -0.5f, 0.5f },{ texLT.x, texLT.y } },
                { { 0.5f, 0.5f },{ texRB.x, texLT.y } },
                { { -0.5f, -0.5f },{ texLT.x, texRB.y } },
                { { 0.5f, -0.5f },{ texRB.x, texRB.y } },
            };

            HRESULT hr = S_OK;
            auto d3dDev = dxDev->GetD3DDevice();
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

            hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->vertexBuf.ReleaseAndGetAddressOf());
            H::System::ThrowIfFailed(hr);
        }

        d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        uint32_t strides = sizeof(TexVertex2D);
        uint32_t offset = 0;

        DirectX::XMMATRIX mvp = DirectX::XMMatrixScaling(2.0f, 2.0f, 1.0f);

        mvp = DirectX::XMMatrixTranspose(mvp);

        d3dCtx->UpdateSubresource(this->vsCBuf.Get(), 0, nullptr, &mvp, 0, 0);

        d3dCtx->IASetInputLayout(this->inputLayout.Get());
        d3dCtx->IASetVertexBuffers(0, 1, this->vertexBuf.GetAddressOf(), &strides, &offset);

        d3dCtx->VSSetConstantBuffers(0, 1, this->vsCBuf.GetAddressOf());
        d3dCtx->VSSetShader(this->vs.Get(), nullptr, 0);

        d3dCtx->PSSetShaderResources(0, 1, this->srv.GetAddressOf());
        d3dCtx->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
        d3dCtx->PSSetShader(this->ps.Get(), nullptr, 0);

        d3dCtx->Draw(4, 0);
    }
}
