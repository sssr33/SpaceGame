#pragma once
#include "GameRendererId.h"
#include "IGameRendererFactory.h"

#include <libhelpers/Math/Box.h>
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

        using OperationScope = ScopedValue<IGameRenderer*, OperationEndDestructor>;
        using AlphaBlendScope = ScopedValue<IGameRenderer*, AlphaBlendPopDestructor>;
        using ScissorScope = ScopedValue<IGameRenderer*, ScissorPopDestructor>;

        virtual ~IGameRenderer() = default;

        virtual IGameRendererFactory& GetFactory() = 0;

        OperationScope OperationBeginScoped();

        // must be called before any render or factory.create operation(s)
        virtual void OperationBegin() = 0;
        // must be called after render or factory.create operation(s)
        virtual void OperationEnd() = 0;

        AlphaBlendScope PushAlphaBlendingScoped(bool premultiplied);
        virtual void PushAlphaBlending(bool premultiplied) = 0;
        virtual void PopAlphaBlending() = 0;

        ScissorScope PushScissorScoped(const Math::IBox& scissorBox);
        virtual void PushScissor(const Math::IBox& scissorBox) = 0;
        virtual void PopScissor() = 0;

        void RenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj);
        void RenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj);
        void RenderText(const std::shared_ptr<ITextRenderer>& obj);

    private:
        virtual void DoRenderBackgroundBrush(const std::shared_ptr<IBackgroundBrushRenderer>& obj) = 0;
        virtual void DoRenderRectangle(const std::shared_ptr<IRectangleRenderer>& obj) = 0;
        virtual void DoRenderText(const std::shared_ptr<ITextRenderer>& obj) = 0;

        template<class ObjT, class FnT>
        void CheckedRender(const ObjT& obj, FnT fn);
    };
}
