#ifndef WIDGET_TITLE_HPP
#define WIDGET_TITLE_HPP

#include <string>

#include "widget.hpp"
#include "text.hpp"
#include "colors.hpp"
#include "button.hpp"

static const char* const kFontFileNameTitle = "data/font.ttf";

static const colors::Color kTitleBackgroundColor = kWidgetDefaultFillColor;
static const colors::Color kTitleBorderColor = kWidgetDefaultBorderColor;
static const float kTitleBorderThickness = kBorderThicknessWidget;

static const float kTitleHeight = 20;
static const float kButtonSize = 10;

static const std::string kButtonHideStr = " --";

static const colors::Color kButtonHideColor = colors::Color(180, 180, 180);

class Title : public Widget {
    private:
        std::string title_;

        Text* text_;
        Widget* goal_;

        dr4::Rectangle* background_;

        Button* hide_button_;

    public:
        Title(const Coordinates& lt_corner, float width,
              hui::State* state, Widget* parent, const std::string& title,
              Widget* goal_widget)
            :Widget(lt_corner, width, kTitleHeight, state, parent), title_(title) {
            goal_ = goal_widget;
            text_ = new Text(
                lt_corner, width, kTitleHeight,
                state, this, title_, kFontFileNameTitle, kTitleHeight
            );

            hide_button_ = new Button(Coordinates(2, width - (kTitleHeight - kButtonSize) / 2 - kButtonSize, (kTitleHeight - kButtonSize) / 2),
                kButtonSize, kButtonSize, kButtonHideStr, kFontFileNameTitle, state, this, kButtonHideColor, kButtonHideColor, kButtonSize * 2, colors::kColorBlack);

            if (state) {
                background_ = state->window_->CreateRectangle();
                background_->SetFillColor(kTitleBackgroundColor);
                background_->SetBorderColor(kTitleBorderColor);
                background_->SetBorderThickness(kTitleBorderThickness);
                background_->SetSize({width - 2 * kTitleBorderThickness, kTitleHeight - 2 * kTitleBorderThickness});
                background_->SetPos({kTitleBorderThickness, kTitleBorderThickness});
            }
        };

        ~Title() {
            if (state->target_widget_ == this) {
                state->target_widget_ = NULL;
            }

            delete hide_button_;
        };

        virtual void Redraw() override {
            background_->DrawOn(*texture);
            text_->Redraw();
            hide_button_->Redraw();
            Widget::Redraw();
        };

        virtual void SetState(hui::State* state_) {
            text_->SetState(state_);
            hide_button_->SetState(state_);

            state = state_;

            Widget::SetState(state_);

            background_ = state_->window_->CreateRectangle();
            background_->SetFillColor(kTitleBackgroundColor);
            background_->SetBorderColor(kTitleBorderColor);
            background_->SetBorderThickness(kTitleBorderThickness);
            background_->SetSize({Widget::GetWidth() - 2 * kTitleBorderThickness, kTitleHeight - 2 * kTitleBorderThickness});
            background_->SetPos({kTitleBorderThickness, kTitleBorderThickness});
        };

        virtual bool OnMouseMove(float shift_x, float shift_y) override {
            goal_->OnMouseMove(shift_x, shift_y);
            return true;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hide_button_->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                goal_->SetHidden(true);
                return true;
            }

            return Widget::OnMousePress(mouse_pos, type);
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hide_button_->OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }

            return Widget::OnMouseRelease(mouse_pos, type);
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (hide_button_->OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc(), delta)) {
                return true;
            }

            return Widget::OnMouseEnter(mouse_pos, delta);
        };
};

#endif // WIDGET_TITLE_HPP
