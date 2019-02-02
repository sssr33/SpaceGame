#pragma once
#include "FreetypeSafe.h"

namespace H {
    class Freetype {
    public:
        static void ThrowIfFailed(FT_Error err);
    };
}
