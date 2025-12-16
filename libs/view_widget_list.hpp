#ifndef VIEW_WIDGET_LIST_HPP
#define VIEW_WIDGET_LIST_HPP

#include "widget.hpp"
#include "widget_title.hpp"
#include "draw.hpp"
#include "button.hpp"

static const size_t kViewWidgetListWidth = 100;
static const colors::Color kButtonViewListColor = colors::Color(49, 49, 49);

class ViewButtonInList : public Button {
    private:
        Widget* target_;

    public:
        ViewButtonInList(const Button& button, Widget* target)
            :Button(button), target_(target) {};

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (Button::OnMousePress(mouse_pos, type)) {
                target_->SetHidden(!target_->GetHidden());

                return true;
            }
            return false;
        };
};

class ViewWidgetList : public WidgetContainer {
    public:
        ViewWidgetList(const Coordinates& lt_corner, hui::State* state, Widget* parent)
            :WidgetContainer(lt_corner - Coordinates(2, 0, kTitleHeight),
                float(kViewWidgetListWidth), kTitleHeight * (state->ui->main_field_->GetChildrenNum() + 1), state, NULL, parent) {
            auto& children = state->ui->main_field_->GetChildren();

            for (size_t i = 0; i < children.size(); i++) {
                WidgetContainer::AddChild(new ViewButtonInList(Button(Coordinates(2, 0, kTitleHeight * (1 + i)), kViewWidgetListWidth, kTitleHeight,
                children[i]->GetName(), kWidgetDefaultFontFileName,
                state, this, kButtonViewListColor, kButtonViewListColor, kTitleHeight, colors::kColorWhite), children[i]));
            }
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            if (WidgetContainer::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }

                return true;
            }
            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            if (Widget::OnMouseRelease(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }
                return true;
            }
            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (hidden_) {
                return false;
            }
            if (Widget::OnMouseEnter(mouse_pos, delta)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }
                return true;
            }
            hidden_ = true;
            return false;
        };

        virtual void SetHidden(bool hidden) override {
            Widget::SetHidden(hidden);

            if (hidden == false) {
                WidgetContainer::ResetChildren();

                WidgetContainer::SetSize({float(kViewWidgetListWidth), kTitleHeight * (state->ui->main_field_->GetChildrenNum() + 1)});

                auto& children = state->ui->main_field_->GetChildren();

                for (size_t i = 0; i < children.size(); i++) {
                    WidgetContainer::AddChild(new ViewButtonInList(Button(Coordinates(2, 0, kTitleHeight * (1 + i)), kViewWidgetListWidth, kTitleHeight,
                    children[i]->GetName(), kWidgetDefaultFontFileName,
                    state, this, kButtonViewListColor, kButtonViewListColor, kTitleHeight, colors::kColorWhite), children[i]));
                }
            }
        };
};

#endif // VIEW_WIDGET_LIST_HPP
