#pragma once
#include "../IDxSymbol.h"

class SimpleDxFontAtlasSymbol : public IDxSymbol {
public:
    SimpleDxFontAtlasSymbol();

    DxSymbolGeometry GetGeometry() override;
    void GetTexture(ID3D11Texture2D **tex) override;

private:

};
