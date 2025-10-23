#ifndef OBJECT_BUTTONS_HPP
#define OBJECT_BUTTONS_HPP

#include "button.hpp"
#include "scene_manager.hpp"

class MoveButton : public Button {
    private:
        Coordinates move_direction_;
        std::function<void(const Coordinates&)> action_;

    public:
        explicit MoveButton(const Button& button, const Coordinates& move_direction,
                            std::function<void(const Coordinates&)> action)
            :Button(button), move_direction_(move_direction), action_(action) {};

        virtual void Action(const SceneManager* scene_manager) {
            scene_manager->MoveCurrentObject(move_direction_);
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, Widget** widget) override {
            if (!Button::OnMousePress(mouse_pos, widget)) {
                return false;
            }
            action_(move_direction_);
            return true;
        };
};

#endif // OBJECT_BUTTONS_HPP
