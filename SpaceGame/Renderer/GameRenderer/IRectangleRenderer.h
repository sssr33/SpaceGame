#pragma once
#include "IGameRendererObject.h"
#include "Geometry/TexVertex.h"

#include <variant>
#include <libhelpers/Math/Vector.h>

namespace GameRenderer {
    struct RectangleGeometryParams {
        float width = 1.f;
        float height = 1.f;
        RGBA8Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
    };

    struct FilledRectangleGeometryParams : public RectangleGeometryParams {
        float roundness = 0.f;
    };

    struct HollowRectangleGeometryParams : public RectangleGeometryParams {
        float outerRoundness = 0.f;
        float innerRoundness = 0.f;
        float thickness = 0.1f;
    };

    struct RectangleTransform {
        Math::Vector2 scale = { 1.f, 1.f };
        Math::Vector3 rotation;
        Math::Vector3 position = { 0.f, 0.f, 1.f };
    };

    class IRectangleRenderer : public IGameRendererObject {
    public:
        using IGameRendererObject::IGameRendererObject;

        using GeometryParams = std::variant<FilledRectangleGeometryParams, HollowRectangleGeometryParams>;

        virtual GeometryParams GetGeometryParams() = 0;
        virtual void SetGeometryParams(const GeometryParams& params) = 0;

        virtual RectangleTransform GetRectangleTransform() = 0;
        virtual void SetRectangleTransform(const RectangleTransform& transform) = 0;
    };
}
