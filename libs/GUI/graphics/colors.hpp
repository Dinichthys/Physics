#ifndef COLORS_HPP
#define COLORS_HPP

#include <stdlib.h>

#include "dr4/math/color.hpp"

#include "vector.hpp"

namespace colors {

    static const float kMaxColor = 255;

    class Color : public dr4::Color, public Coordinates {
        public:
            explicit Color(float red = 0, float green = 0, float blue = 0, float brightness = kMaxColor)
                :dr4::Color(red, green, blue, brightness), Coordinates(3, red, green, blue) {
                a = (brightness > kMaxColor) ? kMaxColor : brightness;
                if (red > kMaxColor) {
                    r = kMaxColor;
                }
                if (green > kMaxColor) {
                    g = kMaxColor;
                }
                if (blue > kMaxColor) {
                    b = kMaxColor;
                }
            };

            explicit Color(const Coordinates& coors)
                :dr4::Color(coors.Clump(0, kMaxColor)[0],
                            coors.Clump(0, kMaxColor)[1],
                            coors.Clump(0, kMaxColor)[2], kMaxColor),
                 Coordinates(coors) {};

            explicit Color(const dr4::Color& color)
                :dr4::Color(color), Coordinates(color.r, color.g, color.b) {};

            uint8_t GetRedPart() const {return r;};
            uint8_t GetGreenPart() const {return g;};
            uint8_t GetBluePart() const {return b;};

            uint8_t GetBrightness() const {return a;};

            void SetBrightness(uint8_t brightness) {a = brightness;};

            Color operator + (const Color& another) const {
                return Color(GetRedPart()    + another.GetRedPart(),
                             GetGreenPart()  + another.GetGreenPart(),
                             GetBluePart()   + another.GetBluePart(),
                             GetBrightness() + another.GetBrightness());
            };

            Color operator * (float val) const {
                return Color(GetRedPart()   * val,
                             GetGreenPart() * val,
                             GetBluePart()  * val);
            };

            Color operator / (float val) const {
                return Color(GetRedPart()   / val,
                             GetGreenPart() / val,
                             GetBluePart()  / val);
            };
    };

    const Color kColorWhite(255, 255, 255);
    const Color kColorRed(255, 0, 0);
    const Color kColorGreen(0, 255, 0);
    const Color kColorBlue(0, 0, 255);
    const Color kColorBrown(150, 75, 0);
    const Color kColorCyan(0, 255, 255);
    const Color kColorYellow(255, 255, 0);
    const Color kColorBlack(19, 19, 19);

};

#endif // COLORS_HPP
