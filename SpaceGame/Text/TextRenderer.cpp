#include "TextRenderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H
#include FT_BBOX_H
#include <libhelpers\HSystem.h>
#include <libhelpersDesktop/Filesystem/StreamFILE.h>

struct Size2D {
    int width = 0;
    int height = 0;

    FT_Pos  mwidth = 0;
    FT_Pos  mheight = 0;
    FT_Pos  mhoriBearingX = 0;
    FT_Pos  mhoriBearingY = 0;

    bool operator==(const Size2D &other) const {
        bool wequ = this->width == other.width;
        bool hequ = this->height == other.height;
        bool equ = wequ && hequ;

        return equ;
    }

    bool EqualNear(const Size2D &other, int error) const {
        int wdiff = std::abs(this->width - other.width);
        int hdiff = std::abs(this->height - other.height);

        bool wequ = wdiff <= error;
        bool hequ = hdiff <= error;
        bool equ = wequ && hequ;

        return equ;
    }

    bool NotEqualNear(const Size2D &other, int error) const {
        return !this->EqualNear(other, error);
    }

    bool EqualNear(const FT_Glyph_Metrics &other, int error) const {
        int wdiff = std::abs(this->mwidth - other.width);
        int hdiff = std::abs(this->mheight - other.height);
        int hbxdiff = std::abs(this->mhoriBearingX - other.horiBearingX);
        int hbydiff = std::abs(this->mhoriBearingY - other.horiBearingY);

        bool wequ = wdiff <= error;
        bool hequ = hdiff <= error;
        bool hbxequ = hbxdiff <= error;
        bool hbyequ = hbydiff <= error;
        bool equ = wequ && hequ && hbxequ && hbyequ;

        return equ;
    }

    bool NotEqualNear(const FT_Glyph_Metrics &other, int error) const {
        return !this->EqualNear(other, error);
    }
};

struct SymbolInfo {
    FT_Glyph_Metrics metrics;
    FT_UShort units_per_EM;
    FT_Short ascender;
    FT_Short descender;
    FT_Short height;
    Size2D bitmapSize;
    FT_Size_RequestRec req;
    FT_Size_Metrics faceMetrics;
    FT_BBox glyphCBox;
    FT_BBox outlineCBox;
    FT_BBox outlineBox;
};

void TestFonts();
void TestFonts2();
SymbolInfo GetSymbolInfo(int load_flags, int size = 0, int symbol = 'A');
Size2D GetPixelSize(FT_BBox box, double size, FT_UShort units_per_EM, int pixHeight);
double GetFontSize2(FT_BBox box, double height, FT_UShort units_per_EM);
double GetFontSize(FT_BBox box, double height, FT_UShort units_per_EM);

TextRenderer::TextRenderer(const std::wstring &str) {
    Filesystem::StreamFILE stream(LR"(Fonts\\NotoSans-Regular.ttf)", Filesystem::StreamFILEMode::ReadOnly);

    auto sz = stream.GetSize();

    std::vector<uint8_t> fontMem;

    fontMem.resize((size_t)sz);

    stream.Read(fontMem.data(), fontMem.size());

    TestFonts2();

    FT_Error error = FT_Err_Ok;
    FT_Library library;
    FT_Face face;

    error = FT_Init_FreeType(&library);
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    error = FT_New_Face(library, "Fonts\\NotoSans-Regular.ttf", 0, &face);
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    if (!(face->face_flags & FT_FACE_FLAG_SCALABLE)) {
        // for now need scalable font
        // error
        int stop = 432;
    }

    error = FT_Set_Char_Size(face, 0, 10 * 64, 0, 0);
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    int load_flags = FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_SCALE;// FT_LOAD_DEFAULT;

    error = FT_Load_Char(
        face,          /* handle to face object */
        'A',   /* glyph index           */
        load_flags);  /* load flags, see below */
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    auto metrics = face->glyph->metrics;
    auto emSize = face->units_per_EM;

    FT_BBox box;

    FT_Outline_Get_BBox(&face->glyph->outline, &box);

    box.xMax = FT_MulFix(box.xMax, 20480);
    box.yMax = FT_MulFix(box.yMax, 20480);

    //const int fracPrec = 1 << 11;
    const int precIncr = (1 << 16) / emSize;

    auto getPixSize = [&](int size, FT_Pos &width, FT_Pos &height) {
        const int half = (emSize * precIncr) / 2;// 64 / 2;
        size *= 64 * precIncr;
        width = ((size * metrics.width + half) / (emSize * precIncr));// +(!!((size * metrics.width + half) % emSize));// *64;
        height = ((size * metrics.height + half) / (emSize * precIncr));// +(!!((size * metrics.height + half) % emSize));// *64;

        int stop = 432;

        /*width = (size * metrics.width / emSize) + !!(size * metrics.width % emSize);
        height = (size * metrics.height / emSize) + !!(size * metrics.height % emSize);*/

        //width = (size * metrics.width / emSize);// +!!(size * metrics.width % emSize);
        //height = (size * metrics.height / emSize);// +!!(size * metrics.height % emSize);

        //width = ((size * emSize * metrics.width) + 32678) / 65536;// +!!(size * metrics.width % emSize);
        //height = ((size * emSize * metrics.height) + 32768) / 65536;// +!!(size * metrics.height % emSize);

        /*width = ((float(size * metrics.width) / (float)emSize) + 0.5f);
        height = ((float(size * metrics.height) / (float)emSize) + 0.5f);*/
    };

    FT_Pos w, h;

    getPixSize(10, w, h);

    float width = 0.f;
    float height = 0.f;
    float width2 = 0.f;
    float height2 = 0.f;

    for (int c = 2; c < 20; c++) {
        for (int sz = 1; sz < 100; sz++) {
            load_flags = FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING;// | FT_LOAD_NO_SCALE;
            error = FT_Set_Char_Size(face, 0, sz * 64, 0, 0);
            error = FT_Load_Char(
                face,          /* handle to face object */
                'A' + c,   /* glyph index           */
                load_flags);  /* load flags, see below */
            if (error != FT_Err_Ok) {
                int stop = 432;
            }

            {
                FT_Glyph_Metrics metrics2 = face->glyph->metrics;
                auto emSize2 = face->units_per_EM;

                width2 = (float)metrics2.width / 64.f;
                height2 = (float)metrics2.height / 64.f;

                float xmin2 = (float)metrics2.horiBearingX / 64.f;
                float ymax2 = (float)metrics2.horiBearingY / 64.f;

                float xmax2 = xmin2 + width2;
                float ymin2 = ymax2 - height2;

                xmin2 = std::floorf(xmin2);
                ymin2 = std::floorf(ymin2);
                xmax2 = std::ceilf(xmax2);
                ymax2 = std::ceilf(ymax2);

                width2 = xmax2 - xmin2;
                height2 = ymax2 - ymin2;

                /*width = (float)(metrics2.width * 64) / (float)emSize2;
                height = (float)(metrics2.height * 64) / (float)emSize2;*/

                /*width = (float)(metrics2.width) / (float)emSize2;
                height = (float)(metrics2.height) / (float)emSize2;

                width *= sz;
                height *= sz;*/

                FT_BBox cbox;

                FT_Outline_Get_CBox(&face->glyph->outline, &cbox);

                float xmin = (float)(cbox.xMin) / 64.f;
                float xmax = (float)(cbox.xMax) / 64.f;
                float ymin = (float)(cbox.yMin) / 64.f;
                float ymax = (float)(cbox.yMax) / 64.f;

                xmin = std::floorf(xmin);
                ymin = std::floorf(ymin);
                xmax = std::ceilf(xmax);
                ymax = std::ceilf(ymax);

                width = xmax - xmin;
                height = ymax - ymin;

                int stop = 324;
            }

            //load_flags = FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING;// | FT_LOAD_NO_SCALE;
            //error = FT_Set_Char_Size(face, 0, sz * 64, 0, 0);
            //error = FT_Load_Char(
            //    face,          /* handle to face object */
            //    'A' + c,   /* glyph index           */
            //    load_flags);  /* load flags, see below */
            //if (error != FT_Err_Ok) {
            //    int stop = 432;
            //}

            error = FT_Render_Glyph(face->glyph,   /* glyph slot  */
                FT_RENDER_MODE_NORMAL); /* render mode */
            if (error != FT_Err_Ok) {
                int stop = 432;
            }

            uint32_t pixW = std::ceilf(width);
            uint32_t pixH = std::ceilf(height);
            uint32_t pixW2 = std::ceilf(width2);
            uint32_t pixH2 = std::ceilf(height2);

            bool sameW = face->glyph->bitmap.width == pixW;
            bool sameH = face->glyph->bitmap.rows == pixH;
            bool sameW2 = face->glyph->bitmap.width == pixW2;
            bool sameH2 = face->glyph->bitmap.rows == pixH2;

            if (!sameW || !sameH || !sameW2 || !sameH2) {
                int stop = 23;
            }

        }
    }

    getPixSize(20, w, h);

    error = FT_Set_Char_Size(face, 0, 20 * 64, 0, 0);
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    error = FT_Load_Char(
        face,          /* handle to face object */
        'A',   /* glyph index           */
        load_flags);  /* load flags, see below */
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    if (face->glyph->format == FT_GLYPH_FORMAT_BITMAP) {
        int stop = 432;
    }

    error = FT_Render_Glyph(face->glyph,   /* glyph slot  */
        FT_RENDER_MODE_NORMAL); /* render mode */
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    auto tmp = face->size;
    int stop = 324;
}

TextRenderer::~TextRenderer() {
}

void TestFonts() {
    int st = 23;

    for (int symbol = 'A'; symbol <= 'Z'; symbol++) {
        auto noScale = GetSymbolInfo(FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_SCALE, 0, symbol);
        double wscale = (double)noScale.metrics.width / (double)noScale.units_per_EM;
        double hscale = (double)noScale.metrics.height / (double)noScale.units_per_EM;
        double bx = (double)noScale.metrics.horiBearingX / (double)noScale.units_per_EM;
        double by = (double)noScale.metrics.horiBearingY / (double)noScale.units_per_EM;

        for (int sz = 1; sz < 100; sz++) {
            auto scaled = GetSymbolInfo(FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING, sz * 64, symbol);

            auto ew2 = ((double)noScale.metrics.width / 64.0) * (double)sz * ((double)noScale.units_per_EM / 65536.0);
            auto eh2 = ((double)noScale.metrics.height / 64.0) * (double)sz * ((double)noScale.units_per_EM / 65536.0);
            auto ex2 = ((double)noScale.metrics.horiBearingX / 64.0) * (double)sz * ((double)noScale.units_per_EM / 65536.0);
            auto ey2 = ((double)noScale.metrics.horiBearingY / 64.0) * (double)sz * ((double)noScale.units_per_EM / 65536.0);

            auto ew3 = ew2 * 64.0;
            auto eh3 = eh2 * 64.0;
            auto ex3 = ex2 * 64.0;
            auto ey3 = ey2 * 64.0;

            auto ew = FT_MulFix(noScale.metrics.width, sz * noScale.units_per_EM);
            auto eh = FT_MulFix(noScale.metrics.height, sz * noScale.units_per_EM);
            auto ex = FT_MulFix(noScale.metrics.horiBearingX, sz * noScale.units_per_EM);
            auto ey = FT_MulFix(noScale.metrics.horiBearingY, sz * noScale.units_per_EM);

            ew = std::ceil(ew3);
            eh = std::ceil(eh3);
            ex = std::ceil(ex3);
            ey = std::ceil(ey3);

            float width2 = (float)ew / 64.f;
            float height2 = (float)eh / 64.f;

            float xmin2 = (float)ex / 64.f;
            float ymax2 = (float)ey / 64.f;

            float xmax2 = xmin2 + width2;
            float ymin2 = ymax2 - height2;

            xmin2 = std::floorf(xmin2);
            ymin2 = std::floorf(ymin2);
            xmax2 = std::ceilf(xmax2);
            ymax2 = std::ceilf(ymax2);

            width2 = xmax2 - xmin2;
            height2 = ymax2 - ymin2;

            Size2D sizeDouble;

            sizeDouble.width = width2;
            sizeDouble.height = height2;

            /*auto ew = wscale * (double)sz;
            auto eh = hscale * (double)sz;
            auto ex = bx * (double)sz;
            auto ey = by * (double)sz;

            auto cew1 = ew * 64.0;
            auto ceh1 = eh * 64.0;
            auto cex1 = ex * 64.0;
            auto cey1 = ey * 64.0;

            auto cew = std::ceil(ew * 64.0);
            auto ceh = std::ceil(eh * 64.0);
            auto cex = std::ceil(ex * 64.0);
            auto cey = std::ceil(ey * 64.0);

            ew = std::ceil(ew * 64.0) / 64.0;
            eh = std::ceil(eh * 64.0) / 64.0;
            ex = std::ceil(ex * 64.0) / 64.0;
            ey = std::ceil(ey * 64.0) / 64.0;

            auto xmin = ex;
            auto ymax = ey;
            auto xmax = xmin + ew;
            auto ymin = ymax - eh;

            xmin = std::floor(xmin);
            ymin = std::floor(ymin);
            xmax = std::ceil(xmax);
            ymax = std::ceil(ymax);

            int expextedWidth = xmax - xmin;
            int expextedHeight = ymax - ymin;*/

            //auto glyphCBoxSize = GetPixelSize(noScale.glyphCBox, sz, noScale.units_per_EM, scaled.bitmapSize.height);
            auto outlineCBoxSize = GetPixelSize(noScale.outlineCBox, sz, noScale.units_per_EM, scaled.bitmapSize.height);
            auto outlineBBoxSize = GetPixelSize(noScale.outlineBox, sz, noScale.units_per_EM, scaled.bitmapSize.height);

            if (
                outlineBBoxSize.NotEqualNear(scaled.bitmapSize, 0) ||
                outlineBBoxSize.NotEqualNear(scaled.metrics, 0)
                )
            {
                int stop = 23;
            }
        }
    }

    //auto pixels_y = FT_MulFix(noScale.metrics.height, 2048);

    int stop = 32;
}

SymbolInfo GetSymbolInfo(int load_flags, int size, int symbol) {
    FT_Error error = FT_Err_Ok;
    FT_Library library;
    FT_Face face;

    error = FT_Init_FreeType(&library);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    error = FT_New_Face(library, "Fonts\\NotoSans-Regular.ttf", 0, &face);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    if (!(face->face_flags & FT_FACE_FLAG_SCALABLE)) {
        // for now need scalable font
        // error
        H::System::DebuggerBreak();
    }

    error = FT_Set_Char_Size(face, 0, size, 0, 0);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    error = FT_Load_Char(
        face,          /* handle to face object */
        symbol,   /* glyph index           */
        load_flags);  /* load flags, see below */
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    error = FT_Render_Glyph(face->glyph,   /* glyph slot  */
        FT_RENDER_MODE_NORMAL); /* render mode */
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    SymbolInfo info;

    info.metrics = face->glyph->metrics;
    info.units_per_EM = face->units_per_EM;
    info.height = face->height;
    info.ascender = face->ascender;
    info.descender = face->descender;
    info.bitmapSize.width = face->glyph->bitmap.width;
    info.bitmapSize.height = face->glyph->bitmap.rows;
    info.faceMetrics = face->size->metrics;
    /*error = FT_Request_Size(face, &info.req);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }*/

    FT_UInt bboxFlags = 0;

    if (load_flags & FT_LOAD_NO_SCALE) {
        bboxFlags |= FT_GLYPH_BBOX_UNSCALED;
    }

    FT_Glyph  glyph;

    error = FT_Get_Glyph(face->glyph, &glyph);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    FT_Glyph_Get_CBox(glyph, bboxFlags, &info.glyphCBox);

    FT_Done_Glyph(glyph);

    error = FT_Outline_Get_BBox(&face->glyph->outline, &info.outlineBox);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    FT_Outline_Get_CBox(&face->glyph->outline, &info.outlineCBox);

    error = FT_Done_Face(face);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    error = FT_Done_FreeType(library);
    if (error != FT_Err_Ok) {
        H::System::DebuggerBreak();
    }

    return info;
}

#define FT_PIX_FLOOR( x )     ( (x) & ~FT_TYPEOF( x )63 )
#define FT_PIX_ROUND( x )     FT_PIX_FLOOR( (x) + 32 )
#define FT_PIX_CEIL( x )      FT_PIX_FLOOR( (x) + 63 )

void TestFonts2() {
    int st = 23;

    for (int symbol = 'A'; symbol <= 'Z'; symbol++) {
        auto noScale = GetSymbolInfo(FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_SCALE, 0, symbol);

        for (int pixSize = 1; pixSize < 1000; pixSize++) {
            auto fntSize1 = GetFontSize2(noScale.outlineCBox, pixSize, noScale.units_per_EM);
            auto fntSize2 = GetFontSize2(noScale.outlineBox, pixSize, noScale.units_per_EM);

            auto fntSize11 = (fntSize1) * 64.0;
            auto fntSize21 = (fntSize2) * 64.0;

            auto fntSize12 = (fntSize11);
            auto fntSize22 = (fntSize21);

            auto scaled1 = GetSymbolInfo(FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING, fntSize12, symbol);
            auto scaled2 = GetSymbolInfo(FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING, fntSize22, symbol);

            auto ps1 = GetPixelSize(noScale.outlineCBox, std::floor(fntSize12) / 64.0, noScale.units_per_EM, 0);
            auto ps2 = GetPixelSize(noScale.outlineBox, std::floor(fntSize22) / 64.0, noScale.units_per_EM, 0);

            // It's ok to have actual bimap larger by 1 pixel because of fractional pixels
            if ((scaled1.bitmapSize.height - pixSize) > 1 || (scaled1.bitmapSize.height - pixSize) < 0) {
                int stop = 23;
            }
            if ((scaled1.bitmapSize.height - pixSize) != 0) {
                int stop = 23;
            }
        }
    }

    st = 43;
}

Size2D GetPixelSize(FT_BBox box, double size, FT_UShort units_per_EM, int pixHeight) {
    Size2D boxSize;

    //auto box2 = box;
    //auto box3 = box;

    double dunitsPerEm = (double)units_per_EM / 65536.0;
    double dymax = ((double)box.yMax / 64.0) * dunitsPerEm;
    double dymin = ((double)box.yMin / 64.0) * dunitsPerEm;
    double dxmax = ((double)box.xMax / 64.0) * dunitsPerEm;
    double dxmin = ((double)box.xMin / 64.0) * dunitsPerEm;

    /*dymax = std::round(dymax * 64.0) / 64.0;
    dymin = std::round(dymin * 64.0) / 64.0;
    dxmax = std::round(dxmax * 64.0) / 64.0;
    dxmin = std::round(dxmin * 64.0) / 64.0;*/

    double dymax2 = dymax * size;
    double dymin2 = dymin * size;
    double dxmax2 = dxmax * size;
    double dxmin2 = dxmin * size;

    dymax2 = std::round(dymax2 * 64.0) / 64.0;
    dymin2 = std::round(dymin2 * 64.0) / 64.0;
    dxmax2 = std::round(dxmax2 * 64.0) / 64.0;
    dxmin2 = std::round(dxmin2 * 64.0) / 64.0;

    //double dheight = dymax2 - dymin2;
    //double dwidtht = dxmax2 - dxmin2;
    double dheight2 = std::ceil(dymax2) - std::floor(dymin2);
    double dwidtht2 = std::ceil(dxmax2) - std::floor(dxmin2);
    //double ddesH = dymax - dymin;

    auto box2 = box;
    auto box3 = box;

    box2.xMin = FT_MulFix(box.xMin, size * units_per_EM);
    box2.yMin = FT_MulFix(box.yMin, size * units_per_EM);
    box2.xMax = FT_MulFix(box.xMax, size * units_per_EM);
    box2.yMax = FT_MulFix(box.yMax, size * units_per_EM);

    //if (size == 39) {
    //    int stop = 23;
    //}

    //auto ymaxTmp = box.yMax;
    ////auto box3ymaxTmp = FT_PIX_CEIL(box3.yMax);
    ///*box2.yMax = FT_DivFix(ymaxTmp, units_per_EM);

    //auto sz3 = FT_DivFix(box2.yMax, box3.yMax * 65536);*/

    //auto designHeight = FT_MulFix(box3.yMax, units_per_EM);

    //auto sz3 = FT_DivFix(ymaxTmp, designHeight * 65536);
    //auto sz4 = (int)(((float)ymaxTmp / 64.0) / ((float)designHeight / 64.0));
    //auto sz5 = dheight / ddesH;
    //auto sz6 = dheight2 / ddesH;
    //auto sz7 = std::floor(sz6);

    //float sizeRestored = (float)box2.yMax / (float)box3.yMax;
    //int sz2 = std::round(sizeRestored);

    //if (sz5 != size) {
    //    if (std::abs(sz5 - size) > 1) {
    //        int stop = 324;
    //    }

    //    if (dheight2 != pixHeight) {
    //        int stop = 23;
    //    }
    //}

    boxSize.mhoriBearingX = box2.xMin;
    boxSize.mhoriBearingY = box2.yMax;
    boxSize.mwidth = box2.xMax - box2.xMin;
    boxSize.mheight = box2.yMax - box2.yMin;

    box3.xMin = FT_PIX_FLOOR(box2.xMin);
    box3.yMin = FT_PIX_FLOOR(box2.yMin);
    box3.xMax = FT_PIX_CEIL(box2.xMax);
    box3.yMax = FT_PIX_CEIL(box2.yMax);

    boxSize.width = box3.xMax - box3.xMin;
    boxSize.height = box3.yMax - box3.yMin;

    float tmpWidth = (float)boxSize.width / 64.0f;
    float tmpHeight = (float)boxSize.height / 64.0f;

    boxSize.width = std::ceilf(tmpWidth);
    boxSize.height = std::ceilf(tmpHeight);

    if (dheight2 != boxSize.height || dwidtht2 != boxSize.width) {
        int stop = 34;
    }

    return boxSize;
}

double GetFontSize2(FT_BBox box, double height, FT_UShort units_per_EM) {
    double fontSize = 0;

    double dunitsPerEm = (double)units_per_EM / 65536.0;
    double dymax = ((double)box.yMax / 64.0) * dunitsPerEm;
    double dymin = ((double)box.yMin / 64.0) * dunitsPerEm;

    double ddesH = dymax - dymin;

    double dfontSize2 = height / ddesH;

    auto box2 = box;
    auto box3 = box;

    //box2.xMin = FT_MulFix(box2.xMin, units_per_EM);
    box2.yMin = FT_MulFix(box2.yMin, units_per_EM);
    //box2.xMax = FT_MulFix(box2.xMax, units_per_EM);
    box2.yMax = FT_MulFix(box2.yMax, units_per_EM);

    auto desH = box2.yMax - box2.yMin;
    auto desHTmp = (int)(((double)desH / 64.0) * 65536.0);
    auto desHTmp2 = desH * 65536 / 64;

    if (desHTmp != desHTmp2) {
        int stop = 324;
    }

    auto fontSize2 = FT_DivFix(height, desHTmp2);

    //box3.yMin = FT_MulFix(box3.yMin, fontSize2 * units_per_EM);
    //box3.yMax = FT_MulFix(box3.yMax, fontSize2 * units_per_EM);
    //auto dy1 = (std::max)(std::abs(box3.yMax), std::abs(box3.yMin)); // max coord
    //auto dy2 = (std::min)(std::abs(box3.yMax), std::abs(box3.yMin)); // min coord
    ////auto my = (std::min)(std::abs(dymax3), std::abs(dymin3));
    //auto dy1Min = FT_PIX_FLOOR(dy1); // begin where <height> will be filled
    ////auto dy1Max = FT_PIX_CEIL(dy1); // end where <height> will be filled
    //auto dy2Min = FT_PIX_FLOOR(dy2); // begin where <height> will be filled
    ////auto dy2Max = FT_PIX_CEIL(dy2); // end where <height> will be filled

    ////return fontSize2;
    return dfontSize2;
}

// DOES NOT WORK AS INTENDED
double GetFontSize(FT_BBox box, double height, FT_UShort units_per_EM) {
    double fontSize = 0;

    double dunitsPerEm = (double)units_per_EM / 65536.0;
    double dymax = ((double)box.yMax / 64.0) * dunitsPerEm;
    double dymin = ((double)box.yMin / 64.0) * dunitsPerEm;

    /*dymax *= 64.0;
    dymin *= 64.0;

    dymax = std::ceil(dymax);
    dymin = std::floor(dymin);

    dymax /= 64.0;
    dymin /= 64.0;*/

    double ddesH = dymax - dymin;
    double ddesH2 = std::ceil(dymax) - std::floor(dymin);
    /*ddesH /= 64.0;*/
    double dheight2 = height;

    double dfontSize2 = dheight2 / ddesH;

    auto dymax3 = dymax * dfontSize2;
    auto dymin3 = dymin * dfontSize2;

    auto dy1 = (std::max)(std::abs(dymax3), std::abs(dymin3)); // max coord
    auto dy2 = (std::min)(std::abs(dymax3), std::abs(dymin3)); // min coord
    //auto my = (std::min)(std::abs(dymax3), std::abs(dymin3));
    auto dy1Min = std::floor(dy1); // begin where <height> will be filled
    auto dy1Max = std::ceil(dy1); // end where <height> will be filled
    auto dy2Min = std::floor(dy2); // begin where <height> will be filled
    auto dy2Max = std::ceil(dy2); // end where <height> will be filled
    //dy2Min = dy2Max - std::ceil(my);

    auto dykT = height - std::floor(height);
    auto dy12 = dy1Min + (dy1Max - dy1Min) * dykT;
    auto dy22 = dy2Min + (dy2Max - dy2Min) * dykT;

    // scales for <dfontSize2> so that min or max coord will get such value so that symbol will have it's height==<height> after rendering
    auto dyk1 = dy12 / dy1;
    auto dyk2 = dy22 / dy2;

    auto dyk = (std::max)(dyk1, dyk2); // don't know if min or max should be used

    auto dfontSize3 = dfontSize2 * dyk;

    auto dymax5 = dymax * dfontSize3;
    auto dymin5 = dymin * dfontSize3;

    int stop = 324;

    return dfontSize3;

    //double dfontSize2 = dheight2 / ddesH;
    //double dfontSize = dheight2 / ddesH2;
    //fontSize = dfontSize;// std::ceil(dfontSize);

    //auto dymax2 = dymax * dfontSize;
    //auto dymin2 = dymin * dfontSize;

    //auto dymax3 = dymax * dfontSize2;
    //auto dymin3 = dymin * dfontSize2;

    //double nddesHK = 1.0 / ddesH;
    //double ndymax = dymax * nddesHK;
    //double ndymin = dymin * nddesHK;

    //double nCenter = (ndymax + ndymin) / 2.0;
    //double toCenterK = 0.5 / nCenter;

    //double dfontSize3 = dfontSize2 / toCenterK;

    //double nddesH = ndymax - ndymin;

    //auto dymax4 = ndymax * dheight2;
    //auto dymin4 = ndymin * dheight2;

    //auto dymax5 = dymax * dfontSize3;
    //auto dymin5 = dymin * dfontSize3;

    //return dheight2;
}