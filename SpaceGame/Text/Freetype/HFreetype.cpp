#include "HFreetype.h"

#include <stdexcept>
#include <string>

namespace H {
    void Freetype::ThrowIfFailed(FT_Error err) {
        if (err != FT_Err_Ok) {
            std::string msg = "Freetype failed. Error code: " + std::to_string(err);
            throw std::runtime_error(msg);
        }
    }
}