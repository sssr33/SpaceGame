#pragma once
#include "GameRendererId.h"
#include "IGameRendererFactory.h"

#include <libhelpers/Math/Vector.h>
#include <libhelpers/Math/Matrix.h>
#include <libhelpers/ScopedValue.h>

namespace GameRenderer {
    class IGameRenderer : public GameRendererId {
    public:
        using GameRendererId::GameRendererId;

        struct OperationEndDestructor {
            void operator()(IGameRenderer* renderer);
        };

        struct AlphaBlendPopDestructor {
            void operator()(IGameRenderer* renderer);
        };

        struct ScissorPopDestructor {
            void operator()(IGameRenderer* renderer);
        };

        struct WorldMatrixPopDestructor {
            void operator()(IGameRenderer* renderer);
        };

        using OperationScope = ScopedValue<IGameRenderer*, OperationEndDestructor>;
        using AlphaBlendScope = ScopedValue<IGameRenderer*, AlphaBlendPopDestructor>;
        using ScissorScope = ScopedValue<IGameRenderer*, ScissorPopDestructor>;
        using WorldMatrixPopScope = ScopedValue<IGameRenderer*, WorldMatrixPopDestructor>;

        virtual ~IGameRenderer() = default;

        virtual IGameRendererFactory& GetFactory() = 0;

        virtual Math::Vector2 GetScreenPixelSize() = 0;

        OperationScope OperationBeginScoped();

        // must be called before any render or factory.create operation(s)
        virtual void OperationBegin() = 0;
        // must be called after render or factory.create operation(s)
        virtual void OperationEnd() = 0;

        AlphaBlendScope PushAlphaBlendingScoped(bool premultiplied);
        virtual void PushAlphaBlending(bool premultiplied) = 0;
        virtual void PopAlphaBlending() = 0;

        ScissorScope PushScissorScoped(const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize);
        virtual void PushScissor(const Math::Vector2& scissorCenter, const Math::Vector2& scissorHalfSize) = 0;
        virtual void PopScissor() = 0;

        WorldMatrixPopScope PushWorldMatrixScoped(const Math::Matrix4& m);
        WorldMatrixPopScope PushWorldMatrixAdditiveScoped(const Math::Matrix4& m);
        virtual void PushWorldMatrix(const Math::Matrix4& m) = 0;
        virtual void PushWorldMatrixAdditive(const Math::Matrix4& m) = 0;
        virtual void PopWorldMatrix() = 0;

        void RenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj);
        void RenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj);
        void RenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj, const std::shared_ptr<ITexture2D>& tex);
        void RenderText(const std::shared_ptr<ITextRenderer>& obj);

    private:
        virtual void DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) = 0;
        virtual void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) = 0;
        virtual void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj, const std::shared_ptr<ITexture2D>& tex) = 0;
        virtual void DoRenderText(const std::shared_ptr<ITextRenderer>& obj) = 0;

        template<class FnT, class... ArgsT>
        void CheckedRender(FnT fn, ArgsT&&... args);
    };
}
