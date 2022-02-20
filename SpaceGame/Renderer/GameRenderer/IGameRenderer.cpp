#include "IGameRenderer.h"

#include <cassert>

namespace GameRenderer {
    void IGameRenderer::RenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) {
        this->CheckedRender(obj, &IGameRenderer::DoRenderBackgroundBrush);
    }

    void IGameRenderer::RenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) {
        this->CheckedRender(obj, &IGameRenderer::DoRenderRectangle);
    }

    void IGameRenderer::RenderText(const std::shared_ptr<ITextRenderer>& obj) {
        this->CheckedRender(obj, &IGameRenderer::DoRenderText);
    }

    template<class ObjT, class FnT>
    void IGameRenderer::CheckedRender(const ObjT& obj, FnT fn) {
        if (!this->IsSameRenderer(*obj)) {
            assert(false);
            return;
        }

        (this->*fn)(obj);
    }
}
