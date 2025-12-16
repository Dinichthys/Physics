#include "color_picker.hpp"
#include "dorisovka.hpp"

ColorPicker::ColorPicker(const Coordinates& lt_corner, hui::State* state,
                           Widget* parent, Dorisovka* cvs)
    :Widget(lt_corner, kColorPickerWidgetWidth, kColorPickerWidgetHeight, state, parent),
        r_text_            (Coordinates(2, 0 + kBorderColorPickerWidth, 0 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, kRColorName,
                            kFontFileNameColorPicker),
        val_r_text_        (Text(Coordinates(2, 0 + kBorderColorPickerWidth, kHeightColorPickerText / 2 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, ColorComponentToUInt8Str(uint8_t(cvs->GetControlsTheme().shapeBorderColor.r)),
                            kFontFileNameColorPicker, kHeightColorPickerText / 2 / kTextScaleColorPicker)),
        g_text_            (Coordinates(2, kWidthColorPicker / 4 + kBorderColorPickerWidth, 0 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, kGColorName,
                            kFontFileNameColorPicker),
        val_g_text_        (Text(Coordinates(2, kWidthColorPicker / 4 + kBorderColorPickerWidth, kHeightColorPickerText / 2 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, ColorComponentToUInt8Str(uint8_t(cvs->GetControlsTheme().shapeBorderColor.g)),
                            kFontFileNameColorPicker, kHeightColorPickerText / 2 / kTextScaleColorPicker)),
        b_text_            (Coordinates(2, kWidthColorPicker / 2 + kBorderColorPickerWidth, 0 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, kBColorName,
                            kFontFileNameColorPicker),
        val_b_text_        (Text(Coordinates(2, kWidthColorPicker / 2 + kBorderColorPickerWidth, kHeightColorPickerText / 2 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, ColorComponentToUInt8Str(uint8_t(cvs->GetControlsTheme().shapeBorderColor.b)),
                            kFontFileNameColorPicker, kHeightColorPickerText / 2 / kTextScaleColorPicker)),
        a_text_            (Coordinates(2, 3 * kWidthColorPicker / 4 + kBorderColorPickerWidth, 0 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, kAColorName,
                            kFontFileNameColorPicker),
        val_a_text_        (Text(Coordinates(2, 3 * kWidthColorPicker / 4 + kBorderColorPickerWidth, kHeightColorPickerText / 2 + kWidthColorPicker + 2 * kBorderColorPickerWidth + kTitleHeight),
                            kWidthColorPicker / 4, kHeightColorPickerText / 2, state,
                            this, ColorComponentToUInt8Str(uint8_t(cvs->GetControlsTheme().shapeBorderColor.a)),
                            kFontFileNameColorPicker, kHeightColorPickerText / 2 / kTextScaleColorPicker)) {
    cvs_ = cvs;

    background_ = state->window_->CreateRectangle();
    background_->SetSize(dr4::Vec2f{kColorPickerWidgetWidth, kColorPickerWidgetHeight} - dr4::Vec2f{kBorderThicknessColorPicker, kBorderThicknessColorPicker} * 2 - dr4::Vec2f{0, kTitleHeight});
    background_->SetPos({kBorderThicknessColorPicker, kBorderThicknessColorPicker + kTitleHeight});
    background_->SetFillColor(kBackgroundColorColorPicker);
    background_->SetBorderColor(kBorderColorColorPicker);
    background_->SetBorderThickness(kBorderThicknessColorPicker);

    color_plate_ = state->window_->CreateImage();
    color_plate_->SetPos({kBorderColorPickerWidth, kBorderColorPickerWidth + kTitleHeight});
    color_plate_->SetSize({kWidthColorPicker, kWidthColorPicker});

    color_bar_ = state->window_->CreateImage();
    color_bar_->SetPos({kWidthColorPicker + kBorderColorPickerWidth * 2, kBorderColorPickerWidth + kTitleHeight});
    color_bar_->SetSize({kWidthColorBar, kWidthColorPicker + kHeightColorPickerText});

    title_ = new Title(Coordinates{3}, kColorPickerWidgetWidth, state, this, kColorPickerName, this);

    caret_ = state->window_->CreateRectangle();
    caret_->SetFillColor(colors::kColorBlack);
    caret_->SetSize({kWidthColorBar, kCaretThickness});
    caret_->SetBorderColor(dr4::Color(0, 0, 0));
    caret_->SetBorderThickness(0);
    caret_->SetPos(color_bar_->GetPos() + dr4::Vec2f{0, RGBToHSB(cvs_->theme_.shapeBorderColor)[0] / 360 * float(kWidthColorPicker + kHeightColorPickerText)});

    dot_ = state->window_->CreateCircle();
    dot_->SetBorderColor(dr4::Color(0, 0, 0));
    dot_->SetBorderThickness(kCaretThickness);
    dot_->SetFillColor(dr4::Color(255, 255, 255));
    dot_->SetRadius(kDotRadius);
    auto color = RGBToHSB(cvs_->theme_.shapeBorderColor);
    dot_->SetCenter(color_plate_->GetPos() + dr4::Vec2f(color[1] * float(kWidthColorPicker), color[2] * float(kWidthColorPicker)));
}

void ColorPicker::Redraw() {
    background_->DrawOn(*texture);

    r_text_.Redraw();
    if (!val_r_text_.GetPrintingInfo()) {
        if (val_r_text_.GetChanged()) {
            cvs_->theme_.shapeBorderColor.r = atoi(val_r_text_.GetText().c_str());
            val_r_text_.SetChanged(false);
        } else {
            val_r_text_.SetText(ColorComponentToUInt8Str(cvs_->theme_.shapeBorderColor.r));
        }
    }
    val_r_text_.Redraw();

    g_text_.Redraw();
    if (!val_g_text_.GetPrintingInfo()) {
        if (val_g_text_.GetChanged()) {
            cvs_->theme_.shapeBorderColor.g = atoi(val_g_text_.GetText().c_str());
            val_g_text_.SetChanged(false);
        } else {
            val_g_text_.SetText(ColorComponentToUInt8Str(cvs_->theme_.shapeBorderColor.g));
        }
    }
    val_g_text_.Redraw();

    b_text_.Redraw();
    if (!val_b_text_.GetPrintingInfo()) {
        if (val_b_text_.GetChanged()) {
            cvs_->theme_.shapeBorderColor.b = atoi(val_b_text_.GetText().c_str());
            val_b_text_.SetChanged(false);
        } else {
            val_b_text_.SetText(ColorComponentToUInt8Str(cvs_->theme_.shapeBorderColor.b));
        }
    }
    val_b_text_.Redraw();

    a_text_.Redraw();
    if (!val_a_text_.GetPrintingInfo()) {
        if (val_a_text_.GetChanged()) {
            cvs_->theme_.shapeBorderColor.a = atoi(val_a_text_.GetText().c_str());
            val_a_text_.SetChanged(false);
        } else {
            val_a_text_.SetText(ColorComponentToUInt8Str(cvs_->theme_.shapeBorderColor.a));
        }
    }
    val_a_text_.Redraw();

    float hue = RGBToHSB(cvs_->theme_.shapeBorderColor)[0];
    for (size_t y = 0; y < kWidthColorPicker; y++) {
        for (size_t x = 0; x < kWidthColorPicker; x++) {
            color_plate_->SetPixel(x, y, HSBToRGB(hue, float(x) / float(kWidthColorPicker), float(y) / float(kWidthColorPicker)));
        }
    }
    color_plate_->DrawOn(*texture);

    for (size_t y = 0; y < kWidthColorPicker + kHeightColorPickerText; y++) {
        for (size_t x = 0; x < kWidthColorBar; x++) {
            color_bar_->SetPixel(x, y, HSBToRGB(float(y) * 360 / float (kWidthColorPicker + kHeightColorPickerText), 1, 1));
        }
    }
    color_bar_->DrawOn(*texture);

    caret_->SetPos(color_bar_->GetPos() + dr4::Vec2f{0, RGBToHSB(cvs_->theme_.shapeBorderColor)[0] / 360 * float(kWidthColorPicker + kHeightColorPickerText)});
    caret_->DrawOn(*texture);

    auto color = RGBToHSB(cvs_->theme_.shapeBorderColor);
    dot_->SetCenter(color_plate_->GetPos() + dr4::Vec2f(color[1] * float(kWidthColorPicker), color[2] * float(kWidthColorPicker)));
    dot_->DrawOn(*texture);

    title_->Redraw();
    Widget::Redraw();
}

bool ColorPicker::OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) {
    if (hidden_) {
        return false;
    }
    Coordinates loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y);
    if ((mouse_pos[0] > relPos.x)
        && (mouse_pos[1] > relPos.y)
        && (mouse_pos[0] < relPos.x + width_)
        && (mouse_pos[1] < relPos.y + height_)) {
        if (title_->OnMousePress(loc_coors, type)) {
            return true;
        }

        if (val_r_text_.OnMousePress(loc_coors, type)) {
            return true;
        }
        if (val_g_text_.OnMousePress(loc_coors, type)) {
            return true;
        }
        if (val_b_text_.OnMousePress(loc_coors, type)) {
            return true;
        }
        if (val_a_text_.OnMousePress(loc_coors, type)) {
            return true;
        }

        auto color_bar_pos = color_bar_->GetPos();
        if ((loc_coors[0] > color_bar_pos.x)
            && (loc_coors[1] > color_bar_pos.y)
            && (loc_coors[0] < color_bar_pos.x + kWidthColorBar)
            && (loc_coors[1] < color_bar_pos.y + kWidthColorPicker + kHeightColorPickerText)) {
            caret_->SetPos({caret_->GetPos().x, loc_coors[1]});
            auto old = RGBToHSB(cvs_->theme_.shapeBorderColor);
            cvs_->theme_.shapeBorderColor = HSBToRGB((loc_coors[1] - color_bar_pos.y) * 360 / (kWidthColorPicker + kHeightColorPickerText), old[1], old[2]);
            state->target_widget_ = NULL;
            return true;
        }

        auto color_plate_pos = color_plate_->GetPos();
        if ((loc_coors[0] > color_plate_pos.x)
            && (loc_coors[1] > color_plate_pos.y)
            && (loc_coors[0] < color_plate_pos.x + kWidthColorPicker)
            && (loc_coors[1] < color_plate_pos.y + kWidthColorPicker)) {
            dot_->SetPos({loc_coors[0], loc_coors[1]});
            auto old = RGBToHSB(cvs_->theme_.shapeBorderColor);
            cvs_->theme_.shapeBorderColor = HSBToRGB(old[0], (loc_coors[0] - color_plate_pos.x) / float(kWidthColorPicker), (loc_coors[1] - color_plate_pos.y) / float(kWidthColorPicker));
            state->target_widget_ = NULL;
            return true;
        }

        state->target_widget_ = NULL;
        return true;
    }

    return false;
};
