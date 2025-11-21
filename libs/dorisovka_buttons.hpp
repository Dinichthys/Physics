#ifndef DORISOVKA_BUTTONS_HPP
#define DORISONKA_BUTTONS_HPP

#include <functional>

#include "button.hpp"

static const float kGeomPrimPanelControlHeight = 50;

class GeomPrimCreationButton : public Button {
    private:
        std::function<void(size_t)> action_;
        size_t id_;

    public:
        explicit GeomPrimCreationButton(const Button& button, const std::function<void(size_t)>& action, size_t id)
            :Button(button), action_(action) {
            id_ = id;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if (!Button::OnMousePress(mouse_pos)) {
                return false;
            }
            action_(id_);
            return true;
        };
};

class GeomPrimToolBar : public PanelControl {
    public:
        GeomPrimToolBar(const Coordinates& lt_corner = Coordinates(3), float width = 0, float height = 0,
                        hui::State* state = NULL,
                        const std::vector<Widget*>* buttons = NULL, Widget* parent = NULL)
            :PanelControl(lt_corner, width, height, state, buttons, parent) {};

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if (!PanelControl::OnMousePress(mouse_pos)) {
                return false;
            }

            if (state->target_widget_ == this) {
                state->target_widget_ = NULL;
            }

            return true;
        };
};

#endif // DORISOVKA_BUTTONS_HPP
