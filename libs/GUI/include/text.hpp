#ifndef TEXT_HPP
#define TEXT_HPP

#include "colors.hpp"

#include "widget.hpp"

class Text : public Widget {
    private:
        dr4::Text* text_;
        std::string font_file_name_;
        std::string str_;
        size_t font_size_;

    public:
        explicit Text(const Coordinates& lt_corner, float width, float height,
                      hui::State* state, Widget* parent = NULL,
                      const std::string& text = "", const std::string& font_file_name = "",
                      float character_size = 0)
            :Widget(lt_corner, width, height, state, parent),
             text_((state == NULL) ? NULL : state->window_->CreateText()),
             font_file_name_(font_file_name),
             str_(text),
             font_size_((character_size > 0) ? character_size : height) {
            if (text_ == NULL) {
            return;
            }
            text_->SetText(text);
            text_->SetPos({0, 0});
            text_->SetFontSize((character_size > 0) ? character_size : height);
            if ((state != NULL) && (font_file_name.compare("") != 0)) {
                dr4::Font* new_font = state->window_->CreateFont();
                if (new_font != NULL) { new_font->LoadFromFile(font_file_name); }
                text_->SetFont(new_font);
            }
        };

        explicit Text(const Text& other)
            :Widget(other),
             text_((state == NULL) ? NULL : state->window_->CreateText()),
             font_file_name_(other.font_file_name_),
             str_(other.str_),
             font_size_(other.font_size_) {
            if (text_ == NULL) {
                return;
            }
            text_->SetText(other.text_->GetText());
            text_->SetPos(other.text_->GetPos());
            text_->SetFontSize(other.text_->GetFontSize());
            if ((state != NULL) && (font_file_name_.compare("") != 0)) {
                dr4::Font* new_font = state->window_->CreateFont();
                if (new_font != NULL) { new_font->LoadFromFile(font_file_name_); }
                text_->SetFont(new_font);
            }
        };

        ~Text() {
            delete text_;
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
                dr4::Font* new_font = state_->window_->CreateFont();
                if (new_font != NULL) { new_font->LoadFromFile(font_file_name_); }
                text_->SetFont(new_font);
            }
            Widget::SetState(state_);
        }

        virtual void Redraw() override {
            texture->Draw(*text_);
            Widget::Redraw();
        };

        void SetText(const std::string& str) {text_->SetText(str); str_ = str;};
        void SetFont(const std::string& font) {
            font_file_name_ = font;
            if (state != NULL) {
                dr4::Font* new_font = state->window_->CreateFont();
                if (new_font != NULL) { new_font->LoadFromFile(font_file_name_); }
                text_->SetFont(new_font);
            }
        };
};

#endif // TEXT_HPP
