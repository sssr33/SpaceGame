#pragma once
#include "../ITexture2D.h"

#include <libhelpers/Dx/Dx.h>

namespace GameRenderer {
    class Texture2DDx : public ITexture2D {
    public:
        Texture2DDx(uint32_t rendererId, DxDevice* dxDev, uint32_t width, uint32_t height, DXGI_FORMAT pixFormat, const void* mem, size_t memSize);

        Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResrouceView() const;

    private:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    };
}
