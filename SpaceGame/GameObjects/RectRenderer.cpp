#include "RectRenderer.h"
#include "..\Geometry\TexVertex.h"
#include "..\Geometry\GeometryFactory.h"

#include <libhelpers\HSystem.h>
#include <libhelpers\HMath.h>
#include <cmath>

RectRenderer::RectRenderer(DxDevice *dxDev) {
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

void RectRenderer::Render(DxDevice *dxDev) {
    auto d3dCtx = dxDev->D3D();

    {
        HRESULT hr = S_OK;
        auto d3dDev = dxDev->GetD3DDevice();
        static float angle2 = 0.0f;

        // geometry
        {
            // * 2 - for antialiasing geometry
            /*uint16_t indices[24 * 3];
            ColorVertex2D vertices[8 * 2];*/

            std::vector<DirectX::XMFLOAT2> pos;
            std::vector<DirectX::XMFLOAT2> texCoords;
            std::vector<DirectX::XMFLOAT2> adjPrev;
            std::vector<DirectX::XMFLOAT2> adjNext;
            std::vector<float> aaDir;
            std::vector<uint32_t> indices2;

            std::vector<ColorVertex2D> vertices;
            std::vector<uint16_t> indices;

            //GeometryFactory::CreateRectangle2(1.0f, 1.0f, 0.01f, 0.0f, 0.0f, pos, adjPrev, adjNext, aaDir, indices2);
            //GeometryFactory::CreateRectangle2(1.0f, 1.0f, 0.01f, std::fabsf(std::sin(angle2)), std::fabsf(std::sin(angle2 * 1.3f)), pos, adjPrev, adjNext, aaDir, indices2);
            //GeometryFactory::CreateRectangle2(1.0f, 1.0f, 0.01f, 0.0f, pos, adjPrev, adjNext, aaDir, indices2);
            //GeometryFactory::CreateRectangleFilled(1.0f, 1.0f, 0.0f, pos, adjPrev, adjNext, aaDir, indices2);
            GeometryFactory::CreateRectangleFilled(1.0f, 1.0f, std::fabsf(std::sin(angle2)), pos, texCoords, adjPrev, adjNext, aaDir, indices2);

            this->indexCount = static_cast<uint32_t>(indices2.size());
            /*GeometryFactory::CreateRectangle(1.0f, 1.0f, 0.01f,
            DataBuffer<DirectX::XMFLOAT2>(vertices, &vertices->pos),
            DataBuffer<DirectX::XMFLOAT2>(vertices, &vertices->aaVec),
            DataBuffer<uint16_t>(indices));*/

            /*for (auto &i : vertices) {
            i.color.r = 0;
            i.color.g = 255;
            i.color.b = 0;
            i.color.a = 255;
            }*/

            for (auto &i : indices2) {
                indices.push_back(i);
            }

            for (size_t i = 0; i < pos.size(); i++) {
                ColorVertex2D v;

                v.color.r(0);
                v.color.g(255);
                v.color.b(0);
                v.color.a(255);
                v.pos = pos[i];
                v.adjPrev = adjPrev[i];
                v.adjNext = adjNext[i];
                v.aaDir = aaDir[i];

                vertices.push_back(v);
            }

            D3D11_BUFFER_DESC bufDesc;
            D3D11_SUBRESOURCE_DATA initData;

            bufDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(ColorVertex2D));
            bufDesc.Usage = D3D11_USAGE_DEFAULT;
            bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bufDesc.CPUAccessFlags = 0;
            bufDesc.MiscFlags = 0;
            bufDesc.StructureByteStride = 0;

            initData.pSysMem = vertices.data();
            initData.SysMemPitch = initData.SysMemSlicePitch = 0;

            hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->vertexBuf.ReleaseAndGetAddressOf());
            H::System::ThrowIfFailed(hr);

            bufDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint16_t));
            bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

            initData.pSysMem = indices.data();

            hr = d3dDev->CreateBuffer(&bufDesc, &initData, this->idxBuf.ReleaseAndGetAddressOf());
            H::System::ThrowIfFailed(hr);
        }

        angle2 += 0.01f;
    }

    uint32_t numViewPorts = 1;
    D3D11_VIEWPORT viewport;

    d3dCtx->RSGetViewports(&numViewPorts, &viewport);

    d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    uint32_t strides = sizeof(ColorVertex2D);
    uint32_t offset = 0;

    struct CBuf {
        DirectX::XMMATRIX mvp;
        DirectX::XMMATRIX toPixels;
        DirectX::XMMATRIX toProjected;
        DirectX::XMVECTOR params;
    };

    CBuf cbufData;
    //DirectX::XMMATRIX mvp;// = DirectX::XMMatrixScaling(2.0f, 2.0f, 1.0f);

    float ar = viewport.Width / viewport.Height;

    static float angle = 0.0f;

    angle += 0.4f;
    //DirectX::XMVector2Dot()
    cbufData.mvp = DirectX::XMMatrixIdentity();
    cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(angle)));
    cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(30.0f)));
    cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixTranslation(0.0f, 0.0f, 1.0f));
    //cbufData.mvp = DirectX::XMMatrixMultiply(cbufData.mvp, DirectX::XMMatrixOrthographicLH(ar * 2.0f, 2.0f, 0.001f, 10.0f));
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

    //DirectX::XMVector3Project
    /*{
        uint16_t indices[24];
        DirectX::XMVECTOR verts[8], verts2[8];

        GeometryFactory::CreateRectangle(1.0f, 1.0f, 0.001f, DataBuffer<DirectX::XMFLOAT2>((DirectX::XMFLOAT2 *)verts, sizeof(DirectX::XMVECTOR)), DataBuffer<uint16_t>(indices));

        for (size_t i = 0; i < 8; i++) {
            verts2[i] = verts[i];
            verts2[i].ZF = 0.5f;
            verts2[i].WF = 1.0f;

            verts2[i] = DirectX::XMVector3Project(verts2[i], 0.0f, 0.0f, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth, mvp, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());
        }

        int stop = 234;
    }*/

    cbufData.toProjected = DirectX::XMMatrixTranspose(cbufData.toProjected);
    cbufData.toPixels = DirectX::XMMatrixTranspose(cbufData.toPixels);
    cbufData.mvp = DirectX::XMMatrixTranspose(cbufData.mvp);

    d3dCtx->UpdateSubresource(this->vsCBuf.Get(), 0, nullptr, &cbufData, 0, 0);

    d3dCtx->IASetInputLayout(this->inputLayout.Get());
    d3dCtx->IASetVertexBuffers(0, 1, this->vertexBuf.GetAddressOf(), &strides, &offset);
    d3dCtx->IASetIndexBuffer(this->idxBuf.Get(), DXGI_FORMAT_R16_UINT, 0);

    d3dCtx->VSSetConstantBuffers(0, 1, this->vsCBuf.GetAddressOf());
    d3dCtx->VSSetShader(this->vs.Get(), nullptr, 0);

    d3dCtx->PSSetShader(this->ps.Get(), nullptr, 0);

    //d3dCtx->RSSetState(this->rsState.Get()); // uncomment to see both sides
    /*d3dCtx->OMSetDepthStencilState(this->dsState.Get(), 0);*/

    d3dCtx->DrawIndexed(this->indexCount, 0, 0);
    //d3dCtx->DrawIndexed(3 * 4, 0, 0);
}