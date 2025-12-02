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

hui::EventResult hui::IdleEvent::Apply(hui::Widget &widget) {
    if ((dynamic_cast<::Widget&>(widget)).OnIdle()) {
        return hui::EventResult::HANDLED;
    }

    return hui::EventResult::UNHANDLED;
}
