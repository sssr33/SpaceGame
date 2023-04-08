#include "IGameRenderer.h"

#include <cassert>

namespace GameRenderer {
    IGameRenderer::OperationScope IGameRenderer::OperationBeginScoped() {
        this->OperationBegin();
        return OperationScope(this, {});
    }

    IGameRenderer::AlphaBlendScope IGameRenderer::PushAlphaBlendingScoped(bool premultiplied) {
        this->PushAlphaBlending(premultiplied);
        return AlphaBlendScope(this, {});
    }

    IGameRenderer::ScissorScope IGameRenderer::PushScissorScoped(const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize) {
        this->PushScissor(scissorCenter, scissorHalfSize);
        return ScissorScope(this, {});
    }

    IGameRenderer::WorldMatrixPopScope IGameRenderer::PushWorldMatrixScoped(const Math::Matrix4& m) {
        this->PushWorldMatrix(m);
        return WorldMatrixPopScope(this, {});
    }

    IGameRenderer::WorldMatrixPopScope IGameRenderer::PushWorldMatrixAdditiveScoped(const Math::Matrix4& m) {
        this->PushWorldMatrixAdditive(m);
        return WorldMatrixPopScope(this, {});
    }

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

    void IGameRenderer::OperationEndDestructor::operator()(IGameRenderer* renderer) {
        assert(renderer);
        renderer->OperationEnd();
    }

    void IGameRenderer::AlphaBlendPopDestructor::operator()(IGameRenderer* renderer) {
        assert(renderer);
        renderer->PopAlphaBlending();
    }

    void IGameRenderer::ScissorPopDestructor::operator()(IGameRenderer* renderer) {
        assert(renderer);
        renderer->PopScissor();
    }

    void IGameRenderer::WorldMatrixPopDestructor::operator()(IGameRenderer* renderer) {
        assert(renderer);
        renderer->PopWorldMatrix();
    }
}
