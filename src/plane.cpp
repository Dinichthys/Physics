#include "plane.hpp"

#include "border.hpp"

Border* Plane::GetBorder() const {
    float size = 2 * (!normal_).GetModule();
    return new Border(Object::GetCenterCoordinates(), 2 * size, 2 * size, 2 * size);
};
