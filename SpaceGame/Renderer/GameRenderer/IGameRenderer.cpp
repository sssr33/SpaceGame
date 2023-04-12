#include "IGameRenderer.h"

#include <cassert>
#include <algorithm>
#include <type_traits>

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
        this->CheckedRender(&IGameRenderer::DoRenderBackgroundBrush, obj);
    }

    void IGameRenderer::RenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) {
        this->CheckedRender([&](auto&&... args) { this->DoRenderRectangle(args...); }, obj);
    }

    void IGameRenderer::RenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj, const std::shared_ptr<ITexture2D>& tex) {
        this->CheckedRender([&](auto&&... args) { this->DoRenderRectangle(args...); }, obj, tex);
    }

    void IGameRenderer::RenderText(const std::shared_ptr<ITextRenderer>& obj) {
        this->CheckedRender(&IGameRenderer::DoRenderText, obj);
    }

    template<class FnT, typename... ArgsT>
    void IGameRenderer::CheckedRender(FnT fn, ArgsT&&... args) {
        if ((!this->IsSameRenderer(*std::forward<ArgsT>(args)) || ...)) {
            assert(false);
            return;
        }

        if constexpr (std::is_member_function_pointer_v<FnT>) {
            (this->*fn)(std::forward<ArgsT>(args)...);
        }
        else {
            fn(std::forward<ArgsT>(args)...);
        }
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
