#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Color.h"
#include "onair/okui/BitmapFont.h"
#include "onair/okui/View.h"

namespace onair {
namespace okui {
namespace views {

class TextView : public View {
public:
    enum class HorizontalAlignment {
        kLeft,
        kCenter,
        kRight,
    };

    enum class VerticalAlignment {
        kBottom,
        kCenter,
        kTop,
    };

    enum class OverflowBehavior {
        kNone,
        kWrap,
        kEllipses,
        kShrink,
    };

    void setAlignment(HorizontalAlignment horizontal, VerticalAlignment vertical = VerticalAlignment::kCenter);
    void setFont(std::shared_ptr<BitmapFont> font, double size);
    void setText(const char* text);
    void setText(const std::string& text) { setText(text.c_str()); }
    void setTextColor(const Color& color);

    template <typename ColorArg, typename... RemColorArgs>
    auto setTextColor(ColorArg&& colorArg, RemColorArgs&&... remColorArgs) -> typename std::enable_if<!std::is_convertible<ColorArg, const Color&>::value>::type {
        setTextColor(Color(std::forward<ColorArg>(colorArg), std::forward<RemColorArgs>(remColorArgs)...));
    }

    void setLetterSpacing(double spacing) { _letterSpacing = spacing; }
    double letterSpacing() const { return _letterSpacing; }

    /**
    * @param texture the name of the texture resource to use
    * @param metadata the name of the metadata resource to use
    */
    void setBitmapFont(const char* texture, const char* metadata, double size);

    void setOverflowBehavior(OverflowBehavior behavior) { _overflowBehavior = behavior;  _computeLines(); }

    double textWidth() const { return _textWidth; }

    enum class Weight {
        kRegular,
        kHeavy,
    };

    void setWeight(Weight weight) { _weight = weight; }
    void setSize(double size) { _fontSize = size; _computeLines(); }

    Point<int> lineColumnPosition(size_t line, size_t column) const;
    std::pair<size_t, size_t> lineColumnAtPosition(int x, int y) const;

    double lineHeight() const;

    virtual void render(const RenderTarget* renderTarget, const Rectangle<int>& area) override;
    virtual void layout() override;
    virtual void windowChanged() override;

private:
    std::shared_ptr<BitmapFont> _font;
    double _fontSize;
    std::string _bitmapFontTexture;
    std::string _bitmapFontMetadata;
    double _bitmapFontSize = 0.0;
    double _letterSpacing = 0.0;

    HorizontalAlignment _horizontalAlignment = HorizontalAlignment::kLeft;
    VerticalAlignment _verticalAlignment = VerticalAlignment::kCenter;
    Weight _weight = Weight::kRegular;
    std::basic_string<BitmapFont::GlyphId> _glyphs;
    std::vector<std::basic_string<BitmapFont::GlyphId>> _lines;
    double _textWidth;
    Color _textColor{0.0};
    OverflowBehavior _overflowBehavior = OverflowBehavior::kEllipses;

    void _computeLines();
    void _renderBitmapText(shaders::DistanceFieldShader* shader);
    double _calcXOffset(const std::basic_string<BitmapFont::GlyphId>& line) const;
    double _calcYOffset() const;
    double _fontScale() const;
};

}}}
