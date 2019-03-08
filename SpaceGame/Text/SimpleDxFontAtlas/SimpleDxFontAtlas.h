#pragma once
#include "../IDxFontAtlas.h"
#include "../IFontRenderer.h"

#include <memory>
#include <wrl.h>
#include <map>
#include <vector>
#include <libhelpers/POTVector.h>

class SimpleDxFontAtlas : public IDxFontAtlas {
public:
    SimpleDxFontAtlas(
        ID3D11Device *d3dDev, ID3D11DeviceContext *d3dCtx,
        std::unique_ptr<IFontRenderer> fontRenderer);

    std::shared_ptr<IDxSymbol> RenderSymbol(
        float fontPixelHeight,
        uint32_t charCode) override;

private:
    typedef POTVector<std::weak_ptr<IDxSymbol>> SymbolData;

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDev;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dCtx;
    std::unique_ptr<IFontRenderer> fontRenderer;

    std::map<uint32_t, SymbolData> symbolData;

    SymbolData &GetSymbolData(uint32_t charCode);

    class Sink : public IFontSink {
    public:

        void Write(
            const PixelPlaneReadonly *plane,
            size_t planeCount) override;

        void SetSegment(const Structs::Rect<uint32_t> &rect) override;

        void SetSize(const Structs::Size<uint32_t> &size);

    private:
        Structs::Size<uint32_t> size;
    };

    class SinkBuilder : public IFontSinkBuilder {
    public:

        size_t SelectFormat(const FontPixelFormat *fmt, size_t count) override;
        void SetSize(const Structs::Size<uint32_t> &size) override;
        void SetMetrics(const NormalizedMetrics &metrics) override;

        IFontSink &GetSink() override;
    private:
        Sink sink;
        NormalizedMetrics metrics;
    };
};