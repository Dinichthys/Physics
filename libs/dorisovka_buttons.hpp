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

#endif // DORISOVKA_BUTTONS_HPP
