#ifndef TEXT_HPP
#define TEXT_HPP

#include "colors.hpp"

#include "widget.hpp"

class Text : public Widget {
    private:
        dr4::Text text_;
        std::string font_file_name_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height,
                      hui::State* state, Widget* parent = NULL,
                      const std::string& text = "", const std::string& font_file_name = "",
                      float character_size = 0)
            :Widget(lt_corner, width, height, state, parent), font_file_name_(font_file_name) {
            text_.text = text;
            text_.pos = {0, 0};
            text_.fontSize = (character_size > 0) ? character_size : height;
            if ((state != NULL) && (font_file_name.compare("") != 0)) {
                text_.font = state->window_->CreateFont();
                text_.font->loadFromFile(font_file_name);
            } else {
                text_.font = NULL;
            }
        };

        explicit Text(const Text& other)
            :Widget(other), font_file_name_(other.font_file_name_) {
            text_ = other.text_;
            if ((state != NULL) && (font_file_name_.compare("") != 0)) {
                text_.font = state->window_->CreateFont();
                text_.font->loadFromFile(font_file_name_);
            } else {
                text_.font = NULL;
            }
        };

        ~Text() {
            if (text_.font != NULL) {
                delete text_.font;
            }
        };

        virtual void SetState(hui::State* state_) {
            Widget::SetState(state_);
            if (font_file_name_.compare("") != 0) {
                if (text_.font != NULL) {
                    delete text_.font;
                }
                text_.font = state->window_->CreateFont();
                text_.font->loadFromFile(font_file_name_);
            }
        }

        virtual void Redraw() override {
            texture->Draw(text_);
            Widget::Redraw();
        };

        void SetText(const std::string& str) {text_.text = str;};
        void SetFont(const std::string& font) {
            font_file_name_ = font;
            if (state != NULL) {
                if (text_.font != NULL) {
                    delete text_.font;
                }
                text_.font = state->window_->CreateFont();
                text_.font->loadFromFile(font_file_name_);
            }
        };
};

#endif // TEXT_HPP
