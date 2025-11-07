#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <stdlib.h>
#include <string>

#include "dr4/math/color.hpp"
#include "dr4/texture.hpp"
#include "dr4/window.hpp"
#include "dr4/event.hpp"

#include "vector.hpp"

namespace graphics {

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

    class FontImpl;

    class Font : public dr4::Font {
        private:
            FontImpl* impl_;

        public:
            explicit Font();
            virtual ~Font();

            virtual void loadFromFile(const std::string& path) const override;

            FontImpl* GetFont() const {return impl_;};
    };

    class Text : public dr4::Text {
        private:
            void* text_;
            void* font_;

        public:
            explicit Text(const std::string& text, const std::string& font_file_name, unsigned char height);

            explicit Text(const Text& other);

            ~Text();

            void SetPosition(const Coordinates& lt_corner) const;

            void SetText(const std::string& text);
            void SetFont(const std::string& font);

            void* GetText() const {return text_;};

            dr4::Rect2f GetBounds() const;
    };

    class VertexArray {
        private:
            void* vertex_array_;

        public:
            explicit VertexArray(size_t size);

            explicit VertexArray(const VertexArray& other);

            ~VertexArray();

            void SetPixelPosition(size_t index, const Coordinates& pos) const;
            void SetPixelColor(size_t index, const Color& color) const;

            void* GetVertexArray() const {return vertex_array_;};
    };

    class RectangleShape : public dr4::Rectangle {
        private:
            void* rectangle_;

        public:
            explicit RectangleShape(const dr4::Vec2f& pos, const dr4::Vec2f& size);

            explicit RectangleShape(const dr4::Rectangle& rect);

            explicit RectangleShape(const RectangleShape& other);

            ~RectangleShape();

            void* GetRectangle() const {return rectangle_;};

            void SetSize(float width, float height);

            void SetPosition(const Coordinates& lt_corner);

            void SetRotation(float angle) const;
            void Rotate(float angle) const;

            void SetOutlineColor(const graphics::Color color);
            void SetFillColor(const graphics::Color color);
    };

    class Image : public dr4::Image {
        private:
            void* image_;
            float width_;
            float height_;

        public:
            explicit Image(float width, float height);

            virtual ~Image();

            virtual void SetPixel(unsigned x, unsigned y, Color color);
            virtual void SetPixel(unsigned x, unsigned y, dr4::Color color) override;
            virtual dr4::Color GetPixel(unsigned x, unsigned y) const override;
            Color GetPixelColor(unsigned x, unsigned y) const;

            virtual void SetSize(dr4::Vec2f size) override;
            virtual dr4::Vec2f GetSize() const override;
            virtual float GetWidth() const override;
            virtual float GetHeight() const override;

            void* GetImage() const {return image_;};
    };

    class Texture : public dr4::Texture {
        private:
            void* texture_;
            float width_;
            float height_;

        public:
            explicit Texture(float width, float height);
            explicit Texture(const Texture& other);

            virtual ~Texture();

            virtual void SetSize(dr4::Vec2f size) override;
            virtual dr4::Vec2f GetSize() const override;
            virtual float GetWidth() const override;
            virtual float GetHeight() const override;

            virtual void Draw(const dr4::Rectangle &rect) override;
            virtual void Draw(const dr4::Text &text) override;
            virtual void Draw(const dr4::Image &img, const dr4::Vec2f &pos) override;
            virtual void Draw(const dr4::Texture &texture, const dr4::Vec2f &pos) override;

            virtual void Display() const;

            virtual void Clear(dr4::Color color);

            void* GetTexture() const {return texture_;};
    };

    const size_t kStartWindowWidth = 720;
    const size_t kStartWindowHeight = 480;

    class RenderWindow : public dr4::Window {
        private:
            std::string title_;

            void* window_;

            float width_;
            float height_;

        public:
            explicit RenderWindow(size_t width = kStartWindowWidth, size_t height = kStartWindowHeight, const char* window_name = "");

            ~RenderWindow();

            float GetStartWidth() const {return width_;};
            float GetStartHeight() const {return height_;};

            float GetWidth() const;
            float GetHeight() const;

            virtual dr4::Vec2f GetSize() const override;
            virtual void SetSize(const ::dr4::Vec2f& size) override;

            virtual void SetTitle(const std::string &title) override;
            virtual const std::string &GetTitle() const override;

            void* GetWindow() const {return window_;};

            virtual dr4::Texture *CreateTexture() override;
            virtual dr4::Image *CreateImage() override;
            virtual dr4::Font *CreateFont() override;

            virtual std::optional<dr4::Event> PollEvent() override;

            Coordinates GetMousePos() const;

            void Draw(const graphics::RectangleShape& rect) const;
            void Draw(const graphics::VertexArray& arr) const;
            void Draw(const graphics::Text& text) const;

            virtual void Draw(const dr4::Texture &texture, dr4::Vec2f pos) override;

            virtual void Display() override;

            virtual bool IsOpen() const override;

            virtual void Open() override;
            virtual void Close() override;

            virtual void Clear(const dr4::Color &color) override;
    };

}

#endif // GRAPHICS_HPP
