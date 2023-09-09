#include "TextRendererDx.h"

#include <limits>
#include <libhelpers\HSystem.h>

namespace GameRenderer {
    TextRendererDx::TextRendererDx(uint32_t rendererId, DxDevice* dxDev, const std::wstring_view& font, float fontSize, const std::wstring_view& text, RGBA8Color textColor)
        : ITextRenderer(rendererId)
        , textColor(textColor)
    {
        HRESULT hr = S_OK;
        auto dwriteFactory = dxDev->GetDwriteFactory();
        Microsoft::WRL::ComPtr<IDWriteTextFormat> textFmt;

        hr = dwriteFactory->CreateTextFormat(font.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us", textFmt.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        hr = dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFmt.Get(), (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)(), this->textLayout.GetAddressOf());
        H::System::ThrowIfFailed(hr);
    }

    void TextRendererDx::Render(DxDevice* dxDev) {
        auto d2dCtx = dxDev->D2D();
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = S_OK;

        hr = d2dCtx->CreateSolidColorBrush(D2D1::ColorF(this->textColor.GetBGRA8(), textColor.af()), brush.GetAddressOf());
        H::System::ThrowIfFailed(hr);

        d2dCtx->BeginDraw();

        auto textOriginPt = D2D1::Point2F(0.f, 20.f);

        d2dCtx->DrawTextLayout(textOriginPt, this->textLayout.Get(), brush.Get());

        DWRITE_TEXT_METRICS metrics = {};

        hr = this->textLayout->GetMetrics(&metrics);
        H::System::ThrowIfFailed(hr);

        auto textRect = D2D1::RectF(metrics.left, metrics.top, metrics.left + metrics.width, metrics.top + metrics.height);

        textRect.left += textOriginPt.x;
        textRect.right += textOriginPt.x;
        textRect.top += textOriginPt.y;
        textRect.bottom += textOriginPt.y;

        d2dCtx->DrawRectangle(textRect, brush.Get());

        hr = d2dCtx->EndDraw();
        H::System::ThrowIfFailed(hr);
    }
}
