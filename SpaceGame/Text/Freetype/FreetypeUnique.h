#pragma once
#include "FreetypeSafe.h"

#include <memory>
#include <type_traits>

struct FT_LibraryDeleter {
    void operator()(FT_Library obj);
};

struct FT_FaceDeleter {
    void operator()(FT_Face obj);
};

typedef std::unique_ptr<std::remove_pointer<FT_Library>::type, FT_LibraryDeleter> FT_LibraryU;
typedef std::unique_ptr<std::remove_pointer<FT_Face>::type, FT_FaceDeleter> FT_FaceU;