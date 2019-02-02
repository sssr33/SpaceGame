#include "FreetypeUnique.h"

void FT_LibraryDeleter::operator()(FT_Library obj) {
    FT_Done_FreeType(obj);
}

void FT_FaceDeleter::operator()(FT_Face obj) {
    FT_Done_Face(obj);
}