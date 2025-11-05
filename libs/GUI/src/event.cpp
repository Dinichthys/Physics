#include "hui/event.hpp"

#include "widget.hpp"

hui::EventResult hui::MouseDownEvent::Apply(hui::Widget &widget) {
    if ((dynamic_cast<::Widget&>(widget)).OnMousePress(Coordinates(2, relPos.x, relPos.y))) {
        return hui::EventResult::HANDLED;
    }

    return hui::EventResult::UNHANDLED;
}

hui::EventResult hui::MouseUpEvent::Apply(hui::Widget &widget) {
    if ((dynamic_cast<::Widget&>(widget)).OnMouseRelease(Coordinates(2, relPos.x, relPos.y))) {
        return hui::EventResult::HANDLED;
    }

    return hui::EventResult::UNHANDLED;
}

hui::EventResult hui::MouseMoveEvent::Apply(hui::Widget &widget) {
    if ((dynamic_cast<::Widget&>(widget)).OnMouseEnter(Coordinates(2, rel.x, rel.y))) {
        return hui::EventResult::HANDLED;
    }

    return hui::EventResult::UNHANDLED;
}

hui::EventResult hui::KeyPressed::Apply(hui::Widget &widget) {
    switch(sym) {
        case dr4::KeyCode::KEYCODE_A : {
            return (dynamic_cast<::Widget&>(widget)).OnLetterA() ? hui::EventResult::HANDLED
                                                                 : hui::EventResult::UNHANDLED;
        }
        case dr4::KeyCode::KEYCODE_D : {
            return (dynamic_cast<::Widget&>(widget)).OnLetterD() ? hui::EventResult::HANDLED
                                                                 : hui::EventResult::UNHANDLED;
        }
        case dr4::KeyCode::KEYCODE_S : {
            return (dynamic_cast<::Widget&>(widget)).OnLetterS() ? hui::EventResult::HANDLED
                                                                 : hui::EventResult::UNHANDLED;
        }
        case dr4::KeyCode::KEYCODE_W : {
            return (dynamic_cast<::Widget&>(widget)).OnLetterW() ? hui::EventResult::HANDLED
                                                                 : hui::EventResult::UNHANDLED;
        }

        case dr4::KeyCode::KEYCODE_RIGHT : {
            return (dynamic_cast<::Widget&>(widget)).OnArrowRight() ? hui::EventResult::HANDLED
                                                                    : hui::EventResult::UNHANDLED;
        }
        case dr4::KeyCode::KEYCODE_LEFT : {
            return (dynamic_cast<::Widget&>(widget)).OnArrowLeft() ? hui::EventResult::HANDLED
                                                                   : hui::EventResult::UNHANDLED;
        }
        case dr4::KeyCode::KEYCODE_UP : {
            return (dynamic_cast<::Widget&>(widget)).OnArrowUp() ? hui::EventResult::HANDLED
                                                                 : hui::EventResult::UNHANDLED;
        }
        case dr4::KeyCode::KEYCODE_DOWN : {
            return (dynamic_cast<::Widget&>(widget)).OnArrowDown() ? hui::EventResult::HANDLED
                                                                   : hui::EventResult::UNHANDLED;
        }

        default:
            return hui::EventResult::UNHANDLED;
    }
}

hui::EventResult hui::IdleEvent::Apply(hui::Widget &widget) {
    if ((dynamic_cast<::Widget&>(widget)).OnIdle()) {
        return hui::EventResult::HANDLED;
    }

    return hui::EventResult::UNHANDLED;
}
