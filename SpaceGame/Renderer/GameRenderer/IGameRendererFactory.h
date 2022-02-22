#pragma once
#include "GameRendererId.h"
#include "IBackgroundBrushRenderer.h"
#include "IRectangleRenderer.h"
#include "ITextRenderer.h"

#include <memory>
#include <string>

namespace GameRenderer {
    class IGameRendererFactory : public GameRendererId {
    public:
        using GameRendererId::GameRendererId;

        virtual ~IGameRendererFactory() = default;

        virtual std::shared_ptr<IBackgroundBrushRenderer> MakeBackgroundBrushRenderer(const std::wstring& texName) = 0;
        virtual std::shared_ptr<IRectangleRenderer> MakeRectangleRenderer() = 0;
        virtual std::shared_ptr<ITextRenderer> MakeTextRenderer(const std::wstring_view& font, float fontSize, const std::wstring_view& text) = 0;
    };
}
