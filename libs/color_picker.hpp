#ifndef COLOR_PICKER_HPP
#define COLOR_PICKER_HPP

#include <string>
#include <algorithm>

#include "widget.hpp"
#include "widget_title.hpp"
#include "editable_text.hpp"
#include "dr4/texture.hpp"

static const std::string kFontFileNameColorPicker = "data/font.ttf";

static const std::string kRColorName = "R";
static const std::string kGColorName = "G";
static const std::string kBColorName = "B";
static const std::string kAColorName = "A";

static const std::string kColorPickerName = "Color Picker";

static const size_t kNumLenColorPicker = 10;

static const size_t kWidthColorBar = 20;
static const size_t kWidthColorPicker = 100;
static const size_t kHeightColorPickerText = 50;
static const size_t kBorderColorPickerWidth = 10;
static const size_t kColorPickerWidgetWidth = kBorderColorPickerWidth * 3 + kWidthColorBar + kWidthColorPicker;
static const size_t kColorPickerWidgetHeight = kBorderColorPickerWidth * 2 + kWidthColorPicker + kHeightColorPickerText + kTitleHeight;

static const size_t kCaretThickness = 1;
static const size_t kDotRadius = 3;

static const size_t kTextScaleColorPicker = 2;

static const colors::Color kBackgroundColorColorPicker = kTitleBackgroundColor;
static const colors::Color kBorderColorColorPicker = kTitleBorderColor;
static const size_t kBorderThicknessColorPicker = kBorderThicknessWidget;

class Dorisovka;

class ColorPicker : public Widget {
    private:
        Text r_text_;
        EditableText val_r_text_;

        Text g_text_;
        EditableText val_g_text_;

        Text b_text_;
        EditableText val_b_text_;

        Text a_text_;
        EditableText val_a_text_;

        Dorisovka* cvs_;

        dr4::Rectangle* background_;

        dr4::Image* color_plate_;

        dr4::Image* color_bar_;

        Title* title_;

        dr4::Rectangle* caret_;

        dr4::Circle* dot_;

    public:
        explicit ColorPicker(const Coordinates& lt_corner, hui::State* state,
                           Widget* parent, Dorisovka* cvs);

        const std::string ColorComponentToUInt8Str(uint8_t component) {
            std::string str;
            char num[kNumLenColorPicker] = "";
            sprintf(num, "%u", component);
            str.append(num);

            return str;
        };

        virtual void Redraw() override;

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type);

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) {
            if (hidden_) {
                return false;
            }

            if (val_r_text_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_g_text_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_b_text_.OnKeyPressed(evt)) {
                return true;
            }
            if (val_a_text_.OnKeyPressed(evt)) {
                return true;
            }

            return false;
        };

        virtual bool OnText(const dr4::Event::TextEvent& evt) {
            if (hidden_) {
                return false;
            }

            if (val_r_text_.OnText(evt)) {
                return true;
            }
            if (val_g_text_.OnText(evt)) {
                return true;
            }
            if (val_b_text_.OnText(evt)) {
                return true;
            }
            if (val_a_text_.OnText(evt)) {
                return true;
            }

            return true;
        };

        colors::Color HSBToRGB(float hue, float saturation, float value) {
            if (saturation == 0) {
                return colors::Color(value * 255, value * 255, value * 255);
            }

            float h = hue / 60.0f;
            int sector = static_cast<int>(h);
            float fraction = h - sector;

            float p = value * (1.0f - saturation);
            float q = value * (1.0f - saturation * fraction);
            float t = value * (1.0f - saturation * (1.0f - fraction));

            float r, g, b;

            switch (sector) {
                case 0:
                    r = value; g = t; b = p;
                    break;
                case 1:
                    r = q; g = value; b = p;
                    break;
                case 2:
                    r = p; g = value; b = t;
                    break;
                case 3:
                    r = p; g = q; b = value;
                    break;
                case 4:
                    r = t; g = p; b = value;
                    break;
                case 5:
                default:
                    r = value; g = p; b = q;
                    break;
            }

            return colors::Color(r * 255, g * 255, b * 255);

                // function hsv_to_rgb(H, S, V):
            // C = V * S
            // H' = H / 60
            // X = C * (1 - abs(H' mod 2 - 1))
            // m = V - C

            // if 0 ≤ H' < 1:   (R₁, G₁, B₁) = (C, X, 0)
            // if 1 ≤ H' < 2:   (R₁, G₁, B₁) = (X, C, 0)
            // if 2 ≤ H' < 3:   (R₁, G₁, B₁) = (0, C, X)
            // if 3 ≤ H' < 4:   (R₁, G₁, B₁) = (0, X, C)
            // if 4 ≤ H' < 5:   (R₁, G₁, B₁) = (X, 0, C)
            // if 5 ≤ H' < 6:   (R₁, G₁, B₁) = (C, 0, X)

            // R = round((R₁ + m) * 255)
            // G = round((G₁ + m) * 255)
            // B = round((B₁ + m) * 255)
            // return (R, G, B)
        };

        Coordinates RGBToHSB(dr4::Color color) {
            float r = float(color.r) / 255;
            float g = float(color.g) / 255;
            float b = float(color.b) / 255;

            float M = std::max(std::max(r, g), b);
            float m = std::min(std::min(r, g), b);

            float C = M - m;

            float B = M;

            float S = 0;
            if (M > kEpsilon) {
                S = C / M;
            }

            float H = 0;
            if (C > kEpsilon) {
                if ((M - r < kEpsilon) && (M - r > -kEpsilon)) {
                    H = 60 * ((g - b) / C);
                } else if ((M - g < kEpsilon) && (M - g > -kEpsilon)) {
                    H = 60 * ((b - r) / C + 2);
                } else {
                    H = 60 * ((r - g) / C + 4);
                }

                if (H < 0) {
                    H = H + 360;
                }
            }

            return Coordinates(3, H, S, B);


//             function rgb_to_hsv(R, G, B):
//                 r = R / 255
//                 g = G / 255
//                 b = B / 255
//
//                 M = max(r, g, b)
//                 m = min(r, g, b)
//                 C = M - m
//
//                 // Яркость
//                 V = M
//
//                 // Насыщенность
//                 if M ≠ 0:
//                     S = C / M
//                 else:
//                     S = 0
//
//                 // Тон
//                 if C = 0:
//                     H = 0
//                 else:
//                     if M = r:
//                         H = 60 * ((g - b) / C)
//                     else if M = g:
//                         H = 60 * ((b - r) / C + 2)
//                     else if M = b:
//                         H = 60 * ((r - g) / C + 4)
//
//                     if H < 0:
//                         H = H + 360
//
//                 // Обычно возвращают H в градусах [0,360), S и V в долях [0,1]
//                 return (H, S, V)
//
//                 // Для процентов: return (H, S * 100, V * 100)
        };
};

#endif // COLOR_PICKER_HPP
