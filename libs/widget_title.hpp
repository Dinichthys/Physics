#ifndef WIDGET_TITLE_HPP
#define WIDGET_TITLE_HPP

#include <string>

#include "widget.hpp"
#include "text.hpp"
#include "colors.hpp"

static const char* const kFontFileNameTitle = "data/font.ttf";

static const colors::Color kTitleBackgroundColor = kWidgetDefaultFillColor;
static const colors::Color kTitleBorderColor = kWidgetDefaultBorderColor;
static const float kTitleBorderThickness = kBorderThicknessWidget;

static const float kTitleHeight = 20;

class Title : public Widget {
    private:
        std::string title_;

        Text* text_;
        Widget* goal_;

        dr4::Rectangle* background_;

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
        };

        virtual void Redraw() override {
            background_->DrawOn(*texture);
            text_->Redraw();
            Widget::Redraw();
        };

        virtual void SetState(hui::State* state_) {
            text_->SetState(state_);
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
};

#endif // WIDGET_TITLE_HPP
