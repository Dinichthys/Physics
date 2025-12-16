#ifndef TOPBAR_HPP
#define TOPBAR_HPP

#include "button.hpp"

class Topbar : public PanelControl {
    public:
        Topbar(const PanelControl& panel)
            :PanelControl(panel) {}

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) {
            if (PanelControl::OnMousePress(mouse_pos, type)) {
                state->target_widget_ = NULL;
            }
            return true;
        };
};

#endif // TOPBAR_HPP
