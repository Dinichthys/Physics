#ifndef TEXT_HPP
#define TEXT_HPP

#include "graphics.hpp"

#include "widget.hpp"

class Text : public Widget {
    private:
        graphics::Text text_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height, Widget* parent = NULL,
                        const std::string& text = "", const std::string& font_file_name = "")
            :Widget(lt_corner, width, height, parent), text_(text, font_file_name, height) {};

        virtual void Draw(graphics::RenderWindow* window) override {
            ASSERT(window != NULL, "");

            Coordinates lt_corner(Widget::GetLTCornerAbs());

            text_.SetPosition(lt_corner);
            window->Draw(text_);
        };

        void SetText(const std::string& str) {text_.SetText(str);};
        void SetFont(const std::string& font) {text_.SetFont(font);};
};

#endif // TEXT_HPP
