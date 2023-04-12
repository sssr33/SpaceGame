#include "Texture2DDx.h"

#include <libhelpers/HSystem.h>

namespace GameRenderer {
    Texture2DDx::Texture2DDx(uint32_t rendererId, DxDevice* dxDev, uint32_t width, uint32_t height, DXGI_FORMAT pixFormat, const void* mem, size_t memSize)
        : ITexture2D(rendererId)
    {
        D3D11_TEXTURE2D_DESC desc = {};

        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 0;
        desc.ArraySize = 1;
        desc.Format = pixFormat;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data = {};

        data.pSysMem = mem;
        data.SysMemPitch = static_cast<UINT>(memSize / height); // width in bytes
        data.SysMemSlicePitch = static_cast<UINT>(memSize);

        HRESULT hr = S_OK;
        auto dev = dxDev->GetD3DDevice();

        hr = dev->CreateTexture2D(&desc, &data, this->texture.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = static_cast<UINT>(-1);
        srvDesc.Texture2D.MostDetailedMip = 0;

        hr = dev->CreateShaderResourceView(this->texture.Get(), &srvDesc, this->srv.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2DDx::GetTexture() const {
        return this->texture;
    }

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture2DDx::GetShaderResrouceView() const {
        return this->srv;
    }
}
