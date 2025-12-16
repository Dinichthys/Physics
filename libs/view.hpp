#ifndef VIEW_HPP
#define VIEW_HPP

#include "draw.hpp"
#include "button.hpp"
#include "view_widget_list.hpp"

static const std::string kViewButtonName = "View";

class ViewButton : public Button {
    private:
        ViewWidgetList* list_;

    public:
        ViewButton(const Coordinates& lt_corner, hui::State* state, Widget* parent)
            :Button(lt_corner, kTopBarButtonWidth, kTitleHeight, kViewButtonName, kFontFileNameTitle, state, parent,
                kTitleBackgroundColor, kTitleBackgroundColor) {list_ = NULL;};

        virtual void SetState(hui::State* state_) override {
            Button::SetState(state_);
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (WidgetContainer::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }

                if (list_) {
                    list_->SetHidden(!(list_->GetHidden()));
                    return true;
                }

                list_ = new ViewWidgetList(Widget::GetLTCornerLoc() + Coordinates(2, 0, kTitleHeight), state, state->ui);
                state->ui->AddChild(list_);

                return true;
            }
            return false;
        };
};

#endif // VIEW_HPP
