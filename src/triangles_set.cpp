#include "triangle.hpp"

#include "border.hpp"

Border* TrianglesSet::GetBorder() const {
    return new Border(Object::GetCenterCoordinates(), kBorderLenTrianglesSet, kBorderLenTrianglesSet, kBorderLenTrianglesSet);
};
