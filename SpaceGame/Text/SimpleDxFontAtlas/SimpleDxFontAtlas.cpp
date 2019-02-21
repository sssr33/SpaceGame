#include "SimpleDxFontAtlas.h"

#include <cassert>

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