#ifndef TOPBAR_HPP
#define TOPBAR_HPP

#include "button.hpp"
#include "widget_title.hpp"

static const size_t kTopBarButtonWidth = 50;

class TopBar : public PanelControl {
    public:
        TopBar(const Coordinates& lt_corner, float width, hui::State* state, std::vector<Widget*>* buttons, Widget* parent)
            :PanelControl(lt_corner, width, kTitleHeight, state, buttons, parent) {}

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) {
            if (PanelControl::OnMousePress(mouse_pos, type)) {
                state->target_widget_ = NULL;
                return true;
            }
            return false;
        };

        virtual void SetState(hui::State* state_) override {
            PanelControl::SetState(state_);
        }
};

#endif // TOPBAR_HPP
