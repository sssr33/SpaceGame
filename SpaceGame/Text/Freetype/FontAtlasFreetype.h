#pragma once
#include "FreetypeUnique.h"
#include "../IFontAtlas.h"
#include "../../Texture/TextureAtlasPage.h"

#include <memory>
#include <cstdint>
#include <vector>
#include <array>
#include <map>
#include <libhelpers/Filesystem/IStreamFS.h>
#include <libhelpers/Structs.h>

class FontAtlasFreetype : public IFontAtlas {
public:
    FontAtlasFreetype(std::vector<uint8_t> fontData, const Structs::SizeU &texPageSize);
    FontAtlasFreetype(Filesystem::IStream &stream, const Structs::SizeU &texPageSize);

    std::shared_ptr<IFontAtlasSymbol> GetDefaultSymbol(uint32_t charCode) override;
    std::shared_ptr<IFontAtlasSymbol> GetSymbol(uint32_t charCode, float fontPixelHeight) override;
    std::shared_ptr<IFontAtlasRenderedSymbol> RenderSymbol(uint32_t charCode, float fontPixelHeight) override;

private:
    class SymbolData {
    public:
        std::weak_ptr<IFontAtlasSymbol> default;

        std::shared_ptr<IFontAtlasRenderedSymbol> GetSymbolData(float fontPixelHeight) const;
        void SetSymbolData(float fontPixelHeight, std::weak_ptr<IFontAtlasRenderedSymbol> data);

    private:
        std::vector<std::weak_ptr<IFontAtlasRenderedSymbol>> potSize;

        static size_t GetPOTSizeIdx(float fontPixelHeight);
    };

    std::vector<uint8_t> fontData;
    FT_LibraryU ftlib;
    FT_FaceU ftFace;

    std::map<uint32_t, SymbolData> symbolData;

    Structs::SizeU texPageSize;
    std::shared_ptr<TextureAtlasPage> lastTexPage;

    SymbolData &GetSymbolData(uint32_t charCode);

    std::shared_ptr<IFontAtlasRenderedSymbol> RenderSymbolInternal(uint32_t charCode, float fontPixelHeight);



    std::shared_ptr<TextureAtlasPage> GetTexPage(const Structs::SizeU &charTexSize);

    static std::vector<uint8_t> ReadStream(Filesystem::IStream &stream);
    static uint32_t RoundFontHeight(float fontPixelHeight);
};