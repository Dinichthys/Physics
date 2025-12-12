#ifndef EDITABLE_TEXT_HPP
#define EDITABLE_TEXT_HPP

#include <string.h>

#include "text.hpp"

#include "dr4/texture.hpp"

static const colors::Color kBorderColorEditableText = colors::kColorWhite;

static const size_t kBorderThickness = 2;

class EditableText : public Text {
    private:
        dr4::Rectangle* border_;

        bool changed_;

        dr4::Rectangle* caret_;
        size_t caret_idx_;

    public:
        EditableText(const Text& text)
            :Text(text) {
            if (state != NULL) {
                border_ = state->window_->CreateRectangle();
                border_->SetBorderColor(kBorderColorEditableText);
                border_->SetBorderThickness(kBorderThickness);
                border_->SetPos(dr4::Vec2f{kBorderThickness, kBorderThickness});
                border_->SetSize(Widget::GetSize() - dr4::Vec2f{kBorderThickness, kBorderThickness} * 2);
                border_->SetFillColor(colors::Color(0, 0, 0, 0));

                caret_ = state->window_->CreateRectangle();
                caret_->SetFillColor(kBorderColorEditableText);
                caret_->SetSize({kBorderThickness, float(font_size_)});
                caret_->SetPos({});
            }
            caret_idx_ = 0;
            changed_ = false;
        };

        virtual void SetState(hui::State* state_) {
            border_ = state_->window_->CreateRectangle();
            border_->SetBorderColor(kBorderColorEditableText);
            border_->SetBorderThickness(kBorderThickness);
            border_->SetPos(dr4::Vec2f{kBorderThickness, kBorderThickness});
            border_->SetSize(Widget::GetSize() - dr4::Vec2f{kBorderThickness, kBorderThickness} * 2);
            border_->SetFillColor(colors::Color(0, 0, 0, 0));

            caret_ = state_->window_->CreateRectangle();
            caret_->SetFillColor(kBorderColorEditableText);
            caret_->SetSize({kBorderThickness, float(font_size_)});
            caret_->SetPos({});

            Text::SetState(state_);
        };

        void SetChanged(bool changed) {
            changed_ = changed;
        }
        bool GetChanged() {
            return changed_;
        }

        bool GetPrintingInfo() {
            return state->target_widget_ == this;
        }

        virtual void Redraw() override {
            if (hidden_) {
                return;
            }
            if (state->target_widget_ == this) {
                texture->Draw(*border_);
                texture->Draw(*caret_);
            }
            Text::Redraw();
        };

        virtual bool OnKeyPressed(const dr4::Event::KeyEvent& evt) {
            if ((hidden_) || (state->target_widget_ != this)) {
                return false;
            }

            switch(evt.sym) {
                case dr4::KeyCode::KEYCODE_ENTER :
                    state->target_widget_ = NULL;
                    return true;

                case dr4::KeyCode::KEYCODE_BACKSPACE :
                    if (caret_idx_ == 0) {
                        return true;
                    }
                    str_ .erase(str_.begin() + caret_idx_ - 1);
                    caret_idx_--;
                    text_->SetText(str_.substr(0, caret_idx_));
                    caret_->SetPos(text_->GetPos() + dr4::Vec2f{text_->GetBounds().x, 0});
                    text_->SetText(str_);
                    changed_ = true;

                    return true;

                case dr4::KeyCode::KEYCODE_RIGHT :
                    if (caret_idx_ == str_.length()) {
                        return true;
                    }
                    caret_idx_++;
                    text_->SetText(str_.substr(0, caret_idx_));
                    caret_->SetPos(text_->GetPos() + dr4::Vec2f{text_->GetBounds().x, 0});
                    text_->SetText(str_);

                    return true;

                case dr4::KeyCode::KEYCODE_LEFT :
                    if (caret_idx_ == 0) {
                        return true;
                    }
                    caret_idx_--;
                    text_->SetText(str_.substr(0, caret_idx_));
                    caret_->SetPos(text_->GetPos() + dr4::Vec2f{text_->GetBounds().x, 0});
                    text_->SetText(str_);

                    return true;

                default:
                    return true;
            }
        };

        virtual bool OnText(const dr4::Event::TextEvent& evt) {
            if ((hidden_) || (state->target_widget_ != this)) {
                return false;
            }

            if ((evt.unicode[0] == '\b' ) || (evt.unicode[0] == '\n' )) {
                return false;
            }

            str_.insert(caret_idx_, evt.unicode);
            caret_idx_ += strlen(evt.unicode);
            text_->SetText(str_.substr(0, caret_idx_));
            caret_->SetPos(text_->GetPos() + dr4::Vec2f{text_->GetBounds().x, 0});
            text_->SetText(str_);
            changed_ = true;

            return true;
        };

        std::string GetText() {
            return str_;
        };

        virtual void SetText(const std::string& str) {
            caret_idx_ = str.length();
            Text::SetText(str);
            caret_->SetPos(text_->GetPos() + dr4::Vec2f{text_->GetBounds().x, 0});
        }

        virtual void Move(float, float) override {};
};

#endif // EDITABLE_TEXT_HPP
