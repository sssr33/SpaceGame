#include "RectangleRendererDx.h"
#include "Geometry\TexVertex.h"
#include "Geometry\GeometryFactory.h"

#include <libhelpers\HSystem.h>
#include <libhelpers\HMath.h>
#include <cmath>

// helper constant for the visitor #3
//template<class> inline constexpr bool always_false_v = false;

namespace GameRenderer {
    RectangleRendererDx::RectangleRendererDx(uint32_t rendererId, DxDevice* dxDev)
        : IRectangleRenderer(rendererId)
    {
        HRESULT hr = S_OK;
        auto d3dDev = dxDev->GetD3DDevice();

        //// geometry
        //{
        //    // * 2 - for antialiasing geometry
        //    /*uint16_t indices[24 * 3];
        //    ColorVertex2D vertices[8 * 2];*/

        //    std::vector<DirectX::XMFLOAT2> pos;
        //    std::vector<DirectX::XMFLOAT2> adjPrev;
        //    std::vector<DirectX::XMFLOAT2> adjNext;
        //    std::vector<float> aaDir;
        //    std::vector<uint32_t> indices2;

        //    std::vector<ColorVertex2D> vertices;
        //    std::vector<uint16_t> indices;

        //    GeometryFactory::CreateRectangle2(1.0f, 1.0f, 0.01f, 0.0f, pos, adjPrev, adjNext, aaDir, indices2);

        //    this->indexCount = indices2.size();

        //    /*GeometryFactory::CreateRectangle(1.0f, 1.0f, 0.01f, 
        //        DataBuffer<DirectX::XMFLOAT2>(vertices, &vertices->pos),
        //        DataBuffer<DirectX::XMFLOAT2>(vertices, &vertices->aaVec),
        //        DataBuffer<uint16_t>(indices));*/

        //    /*for (auto &i : vertices) {
        //        i.color.r = 0;
        //        i.color.g = 255;
        //        i.color.b = 0;
        //        i.color.a = 255;
        //    }*/

        //    for (auto &i : indices2) {
        //        indices.push_back(i);
        //    }

        //    for (size_t i = 0; i < pos.size(); i++) {
        //        ColorVertex2D v;

        //        v.color.r = 0;
        //        v.color.g = 255;
        //        v.color.b = 0;
        //        v.color.a = 255;
        //        v.pos = pos[i];
        //        v.adjPrev = adjPrev[i];
        //        v.adjNext = adjNext[i];
        //        v.aaDir = aaDir[i];

        //        vertices.push_back(v);
        //    }

        //    D3D11_BUFFER_DESC bufDesc;
        //    D3D11_SUBRESOURCE_DATA initData;

        //    bufDesc.ByteWidth = vertices.size() * sizeof(ColorVertex2D);
        //    bufDesc.Usage = D3D11_USAGE_DEFAULT;
        //    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        //    bufDesc.CPUAccessFlags = 0;
        //    bufDesc.MiscFlags = 0;
        //    bufDesc.StructureByteStride = 0;

        //    initData.pSysMem = vertices.data();
        //    initData.SysMemPitch = initData.SysMemSlicePitch = 0;

        //    hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->vertexBuf.ReleaseAndGetAddressOf());
        //    H::System::ThrowIfFailed(hr);

        //    bufDesc.ByteWidth = indices.size() * sizeof(uint16_t);
        //    bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        //    initData.pSysMem = indices.data();

        //    hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->idxBuf.ReleaseAndGetAddressOf());
        //    H::System::ThrowIfFailed(hr);
        //}

        // vertex shader and input layout
        {
            auto data = H::System::LoadPackageFile(L"Color2DVs.cso");

            hr = d3dDev->CreateVertexShader(data.data(), data.size(), nullptr, this->vs.GetAddressOf());
            H::System::ThrowIfFailed(hr);

            D3D11_BUFFER_DESC bufDesc;

            bufDesc.ByteWidth = sizeof(DirectX::XMMATRIX) * 3 + sizeof(DirectX::XMVECTOR);
            bufDesc.Usage = D3D11_USAGE_DEFAULT;
            bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bufDesc.CPUAccessFlags = 0;
            bufDesc.MiscFlags = 0;
            bufDesc.StructureByteStride = 0;

            hr = d3dDev->CreateBuffer(&bufDesc, nullptr, this->vsCBuf.GetAddressOf());
            H::System::ThrowIfFailed(hr);

            D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            hr = d3dDev->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), data.data(), data.size(), this->inputLayout.GetAddressOf());
            H::System::ThrowIfFailed(hr);
        }

        // pixel shader
        {
            auto data = H::System::LoadPackageFile(L"Color2DPs.cso");

            hr = d3dDev->CreatePixelShader(data.data(), data.size(), nullptr, this->ps.GetAddressOf());
            H::System::ThrowIfFailed(hr);
        }

        {
            D3D11_RASTERIZER_DESC rsDesc;

            rsDesc.FillMode = D3D11_FILL_SOLID;
            rsDesc.CullMode = D3D11_CULL_NONE;
            rsDesc.FrontCounterClockwise = FALSE;
            rsDesc.DepthBias = 0;
            rsDesc.SlopeScaledDepthBias = 0.0f;
            rsDesc.DepthBiasClamp = 0.0f;
            rsDesc.DepthClipEnable = FALSE;
            rsDesc.ScissorEnable = FALSE;
            rsDesc.MultisampleEnable = FALSE;
            rsDesc.AntialiasedLineEnable = FALSE;

            hr = d3dDev->CreateRasterizerState(&rsDesc, this->rsState.GetAddressOf());
            H::System::ThrowIfFailed(hr);
        }

        /*{
            D3D11_DEPTH_STENCIL_DESC dsDesc;

            dsDesc.DepthEnable = FALSE;
            dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
            dsDesc.StencilEnable = FALSE;
            dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
            dsDesc.FrontFace.StencilFunc = dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            dsDesc.FrontFace.StencilDepthFailOp = dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            dsDesc.FrontFace.StencilPassOp = dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            dsDesc.FrontFace.StencilFailOp = dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

            hr = d3dDev->CreateDepthStencilState(&dsDesc, this->dsState.GetAddressOf());
            H::System::ThrowIfFailed(hr);
        }*/
    }

    RectangleRendererDx::GeometryParams RectangleRendererDx::GetGeometryParams() {
        return this->geomParams;
    }

    void RectangleRendererDx::SetGeometryParams(const GeometryParams& params) {
        this->geomParams = params;
        this->geomParamsUpdated = true;
    }

    RectangleTransform RectangleRendererDx::GetRectangleTransform() {
        return this->transform;
    }

    void RectangleRendererDx::SetRectangleTransform(const RectangleTransform& transform) {
        this->transform = transform;
        this->transformUpdated = true;
    }

    void RectangleRendererDx::Render(DxDevice* dxDev) {
        auto d3dCtx = dxDev->D3D();

        CheckGeometry(dxDev);
        CheckTransform(dxDev);

        uint32_t strides = sizeof(ColorVertex2D);
        uint32_t offset = 0;

        d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        d3dCtx->IASetInputLayout(this->inputLayout.Get());
        d3dCtx->IASetVertexBuffers(0, 1, this->vertexBuf.GetAddressOf(), &strides, &offset);
        d3dCtx->IASetIndexBuffer(this->idxBuf.Get(), DXGI_FORMAT_R16_UINT, 0);

        d3dCtx->VSSetConstantBuffers(0, 1, this->vsCBuf.GetAddressOf());
        d3dCtx->VSSetShader(this->vs.Get(), nullptr, 0);

        d3dCtx->PSSetShader(this->ps.Get(), nullptr, 0);

        //d3dCtx->RSSetState(this->rsState.Get()); // uncomment to see both sides
        /*d3dCtx->OMSetDepthStencilState(this->dsState.Get(), 0);*/

        d3dCtx->DrawIndexed(this->indexCount, 0, 0);
    }

    void RectangleRendererDx::CheckGeometry(DxDevice* dxDev) {
        if (!this->geomParamsUpdated) {
            return;
        }

        this->geomParamsUpdated = false;

        std::vector<DirectX::XMFLOAT2> pos;
        std::vector<DirectX::XMFLOAT2> adjPrev;
        std::vector<DirectX::XMFLOAT2> adjNext;
        std::vector<float> aaDir;
        std::vector<uint32_t> indices2;
        RGBA8Color color;

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, FilledRectangleGeometryParams>) {
                const FilledRectangleGeometryParams& params = arg;
                color = params.color;
                GeometryFactory::CreateRectangleFilled(params.width, params.height, params.roundness, pos, adjPrev, adjNext, aaDir, indices2);
            }
            else if constexpr (std::is_same_v<T, HollowRectangleGeometryParams>) {
                const HollowRectangleGeometryParams& params = arg;
                color = params.color;
                GeometryFactory::CreateRectangle2(params.width, params.height, params.thickness, params.outerRoundness, params.innerRoundness, pos, adjPrev, adjNext, aaDir, indices2);
            }
            else {
                static_assert(false, "non-exhaustive visitor!");
            }
        }, this->geomParams);

        HRESULT hr = S_OK;
        auto d3dDev = dxDev->GetD3DDevice();

        std::vector<ColorVertex2D> vertices;
        std::vector<uint16_t> indices;

        this->indexCount = indices2.size();

        for (auto& i : indices2) {
            indices.push_back(i);
        }

        for (size_t i = 0; i < pos.size(); i++) {
            ColorVertex2D v;

            v.color = color;
            v.pos = pos[i];
            v.adjPrev = adjPrev[i];
            v.adjNext = adjNext[i];
            v.aaDir = aaDir[i];

            vertices.push_back(v);
        }

        D3D11_BUFFER_DESC bufDesc;
        D3D11_SUBRESOURCE_DATA initData;

        bufDesc.ByteWidth = vertices.size() * sizeof(ColorVertex2D);
        bufDesc.Usage = D3D11_USAGE_DEFAULT;
        bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufDesc.CPUAccessFlags = 0;
        bufDesc.MiscFlags = 0;
        bufDesc.StructureByteStride = 0;

        initData.pSysMem = vertices.data();
        initData.SysMemPitch = initData.SysMemSlicePitch = 0;

        hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->vertexBuf.ReleaseAndGetAddressOf());
        H::System::ThrowIfFailed(hr);

        bufDesc.ByteWidth = indices.size() * sizeof(uint16_t);
        bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        initData.pSysMem = indices.data();

        hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->idxBuf.ReleaseAndGetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    void RectangleRendererDx::CheckTransform(DxDevice* dxDev) {
        auto d3dCtx = dxDev->D3D();

        uint32_t numViewPorts = 1;
        D3D11_VIEWPORT viewport;

        d3dCtx->RSGetViewports(&numViewPorts, &viewport);

        if (!this->transformUpdated && (this->lastScreenWidth != viewport.Width || this->lastScreenHeight != viewport.Height)) {
            this->transformUpdated = true;
        }

        if (!this->transformUpdated) {
            return;
        }

        this->transformUpdated = false;
        this->lastScreenWidth = viewport.Width;
        this->lastScreenHeight = viewport.Height;

        struct CBuf {
            DirectX::XMMATRIX mvp;
            DirectX::XMMATRIX toPixels;
            DirectX::XMMATRIX toProjected;
            DirectX::XMVECTOR params;
        };

        CBuf cbufData;

        const float ar = viewport.Width / viewport.Height;
        const auto& scale = this->transform.scale;
        const auto& rot = this->transform.rotation;
        const auto& pos = this->transform.position;

        cbufData.mvp = DirectX::XMMatrixScaling(scale.x, scale.y, 1.f);
        cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z));
        cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));
        cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.f), ar, 0.001f, 10.0f));

        {
            const float HalfViewportWidth = viewport.Width * 0.5f;
            const float HalfViewportHeight = viewport.Height * 0.5f;

            DirectX::XMVECTOR Scale = DirectX::XMVectorSet(HalfViewportWidth, -HalfViewportHeight, 0.0f, 0.0f);
            DirectX::XMVECTOR Offset = DirectX::XMVectorSet(viewport.TopLeftX + HalfViewportWidth, viewport.TopLeftY + HalfViewportHeight, 0.0f, 0.0f);

            DirectX::XMMATRIX tmp = DirectX::XMMatrixMultiply(
                DirectX::XMMatrixScaling(Scale.XF, Scale.YF, 1.0f),
                DirectX::XMMatrixTranslation(Offset.XF, Offset.YF, 0.0f));

            cbufData.toPixels = tmp;// DirectX::XMMatrixMultiply(cbufData.mvp, tmp);
            //cbufData.toProjected = DirectX::XMMatrixInverse(nullptr, tmp);
            //cbufData.toProjected = DirectX::XMMatrixInverse(nullptr, cbufData.mvp);

            //cbufData.toProjected = DirectX::XMMatrixOrthographicLH(ar * 2.0f, 2.0f, 0.001f, 10.0f);
            cbufData.toProjected = tmp;// DirectX::XMMatrixMultiply(cbufData.toProjected, tmp);
            cbufData.toProjected = DirectX::XMMatrixInverse(nullptr, cbufData.toProjected);

            /*auto tmpVec = DirectX::XMVectorSet(HalfViewportWidth, HalfViewportHeight, 0.0f, 1.0f);

            auto tmpVec2 = DirectX::XMVector3Transform(tmpVec, cbufData.toProjected);*/

            cbufData.params.XF = HalfViewportWidth;
            cbufData.params.YF = HalfViewportHeight;
        }

        cbufData.toProjected = DirectX::XMMatrixTranspose(cbufData.toProjected);
        cbufData.toPixels = DirectX::XMMatrixTranspose(cbufData.toPixels);
        cbufData.mvp = DirectX::XMMatrixTranspose(cbufData.mvp);

        d3dCtx->UpdateSubresource(this->vsCBuf.Get(), 0, nullptr, &cbufData, 0, 0);
    }
}
