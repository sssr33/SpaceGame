#include "TextRenderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#include <libhelpers\HSystem.h>

TextRenderer::TextRenderer(const std::wstring &str) {
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

    load_flags = FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING;

    error = FT_Load_Char(
        face,          /* handle to face object */
        'A',   /* glyph index           */
        load_flags);  /* load flags, see below */
    if (error != FT_Err_Ok) {
        int stop = 432;
    }

    error = FT_Render_Glyph(face->glyph,   /* glyph slot  */
        FT_RENDER_MODE_NORMAL); /* render mode */
    if (error != FT_Err_Ok) {
        int stop = 432;
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