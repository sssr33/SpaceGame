#include "SimpleDxFontAtlas.h"

#include <cassert>
#include <algorithm>

SimpleDxFontAtlas::SimpleDxFontAtlas(
    ID3D11Device *d3dDev, ID3D11DeviceContext *d3dCtx,
    std::unique_ptr<IFontRenderer> fontRenderer)
    : d3dDev(d3dDev), d3dCtx(d3dCtx)
    , fontRenderer(std::move(fontRenderer))
{}

std::shared_ptr<IDxSymbol> SimpleDxFontAtlas::RenderSymbol(
    float fontPixelHeight,
    uint32_t charCode)
{
    auto &symbolData = this->GetSymbolData(charCode);
    auto pair = symbolData.Get(fontPixelHeight);
    auto symbol = pair.val.lock();

    if (!symbol) {
        SinkBuilder sinkBuilder;
        this->fontRenderer->Render(pair.idx, charCode, sinkBuilder);
    }

    return symbol;
}

SimpleDxFontAtlas::SymbolData &SimpleDxFontAtlas::GetSymbolData(uint32_t charCode) {
    auto find = this->symbolData.find(charCode);

    if (find != this->symbolData.end()) {
        return find->second;
    }

    auto added = this->symbolData.insert(std::make_pair(charCode, SymbolData()));
    assert(added.second); // inserted

    return added.first->second;
}




void SimpleDxFontAtlas::Sink::Write(
    const PixelPlaneReadonly *plane,
    size_t planeCount)
{
    assert(false); // need to impl
}

void SimpleDxFontAtlas::Sink::SetSegment(const Structs::Rect<uint32_t> &rect) {
    int stop = 324;
    assert(false); // need to impl
}

void SimpleDxFontAtlas::Sink::SetSize(const Structs::Size<uint32_t> &size) {
    int stop = 324;
    assert(false); // need to impl
}




size_t SimpleDxFontAtlas::SinkBuilder::SelectFormat(const FontPixelFormat *fmt, size_t count) {
    auto beg = fmt;
    auto end = fmt + count;

    auto selected = std::find(beg, end, FontPixelFormat::Gray8);

    if (selected == end) {
        throw std::runtime_error("SimpleDxFontAtlas::SinkBuilder::SelectFormat");
    }

    auto idx = (size_t)(selected - beg);
    return idx;
}

void SimpleDxFontAtlas::SinkBuilder::SetSize(const Structs::Size<uint32_t> &size) {
    this->sink.SetSize(size);
}

void SimpleDxFontAtlas::SinkBuilder::SetMetrics(const NormalizedMetrics &metrics) {
    this->metrics = metrics;
}

IFontSink &SimpleDxFontAtlas::SinkBuilder::GetSink() {
    return this->sink;
}