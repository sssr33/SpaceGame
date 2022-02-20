#pragma once
#include "GameRendererId.h"
#include "IGameRendererFactory.h"

namespace GameRenderer {
    class IGameRenderer : public GameRendererId {
    public:
        using GameRendererId::GameRendererId;

        virtual ~IGameRenderer() = default;

        virtual IGameRendererFactory& GetFactory() = 0;

        // must be called before any render or factory.create operation(s)
        virtual void OperationBegin() = 0;
        // must be called after render or factory.create operation(s)
        virtual void OperationEnd() = 0;

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
