#ifndef TEXT_HPP
#define TEXT_HPP

#include "colors.hpp"

#include "widget.hpp"

class Text : public Widget {
    public:
        dr4::Text* text_;
        dr4::Font* font_;
        std::string font_file_name_;
        std::string str_;
        size_t font_size_;

        colors::Color text_color_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height,
                      hui::State* state, Widget* parent = NULL,
                      const std::string& text = "", const std::string& font_file_name = "",
                      float character_size = 0, colors::Color text_color = colors::kColorWhite)
            :Widget(lt_corner, width, height, state, parent),
             text_((state == NULL) ? NULL : state->window_->CreateText()),
             font_((state == NULL) ? NULL : state->window_->CreateFont()),
             font_file_name_(font_file_name),
             str_(text),
             font_size_((character_size > 0) ? character_size : height),
             text_color_(text_color) {
            if (text_ == NULL) {
            return;
            }
            text_->SetText(text);
            text_->SetPos({0, 0});
            text_->SetFontSize((character_size > 0) ? character_size : height);
            text_->SetColor(text_color);
            text_->SetVAlign(dr4::Text::VAlign::TOP);
            if ((state != NULL) && (font_file_name.compare("") != 0)) {
                if (font_ != NULL) { font_->LoadFromFile(font_file_name); }
                text_->SetFont(font_);
            }
        };

        explicit Text(const Text& other)
            :Widget(other),
             text_((state == NULL) ? NULL : state->window_->CreateText()),
             font_((state == NULL) ? NULL : state->window_->CreateFont()),
             font_file_name_(other.font_file_name_),
             str_(other.str_),
             font_size_(other.font_size_),
             text_color_(other.text_color_) {
            if (text_ == NULL) {
                return;
            }
            text_->SetText(other.text_->GetText());
            text_->SetPos(other.text_->GetPos());
            text_->SetFontSize(other.text_->GetFontSize());
            text_->SetColor(other.text_->GetColor());
            text_->SetVAlign(dr4::Text::VAlign::TOP);
            if ((state != NULL) && (font_file_name_.compare("") != 0)) {
                if (font_ != NULL) { font_->LoadFromFile(font_file_name_); }
                text_->SetFont(font_);
            }
        };

        ~Text() {
            delete text_;
            delete font_;
        };

        virtual void SetState(hui::State* state_) {
            if (font_file_name_.compare("") != 0) {
                if (text_ != NULL) {
                    Widget::SetState(state_);
                    return;
                }
                text_ = state_->window_->CreateText();
                text_->SetText(str_);
                text_->SetFontSize(font_size_);
                text_->SetPos({0, 0});
                text_->SetColor(text_color_);
                text_->SetVAlign(dr4::Text::VAlign::TOP);
                font_ = state_->window_->CreateFont();
                if (font_ != NULL) { font_->LoadFromFile(font_file_name_); }
                text_->SetFont(font_);
            }
            Widget::SetState(state_);
        }

        virtual void Redraw() override {
            texture->Draw(*text_);
            Widget::Redraw();
        };

        virtual void SetText(const std::string& str) {text_->SetText(str); str_ = str;};
        void SetFont(const std::string& font) {
            font_file_name_ = font;
            if (state != NULL) {
                if (font_ != NULL) { font_->LoadFromFile(font_file_name_); }
                text_->SetFont(font_);
            }
        };
};

#endif // TEXT_HPP
