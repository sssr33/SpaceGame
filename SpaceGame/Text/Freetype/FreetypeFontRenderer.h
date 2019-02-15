#pragma once
#include "FreetypeUnique.h"
#include "../IFontRenderer.h"

#include <vector>
#include <cstdint>
#include <libhelpers/Filesystem/IStreamFS.h>

class FreetypeFontRenderer : public IFontRenderer {
public:
    FreetypeFontRenderer(std::vector<uint8_t> fontData);
    FreetypeFontRenderer(Filesystem::IStream &stream);

    void Render(
        float fontPixelHeight,
        uint32_t charCode,
        IFontSinkBuilder &sinkBuilder) override;

private:
    std::vector<uint8_t> fontData;
    FT_LibraryU ftlib;
    FT_FaceU ftFace;

    NormalizedMetrics GetMetrics() const;
    static std::vector<uint8_t> ReadStream(Filesystem::IStream &stream);
};

