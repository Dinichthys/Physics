#include "scrollbar.hpp"

#include "widget.hpp"

bool ArrowScrollBar::OnMousePress(const Coordinates& mouse_pos) {
    if (!Button::OnMousePress(mouse_pos)) {
        return false;
    }

    ScrollBar* parent = dynamic_cast<ScrollBar*>(Widget::GetParent());
    if(parent == NULL) {
        return true;
    }
    parent->MoveThumb(delta_);

    return true;
};

void Thumb::Move(float shift_x, float shift_y) {
    ScrollBar* parent = dynamic_cast<ScrollBar*>(Widget::GetParent());
    if(parent == NULL) {
        return;
    }

    float parent_width = parent->GetWidth();
    float parent_height = parent->GetHeight();

    const Coordinates& lt_corner = Widget::GetLTCornerLoc();
    float width = Widget::GetWidth();
    float height = Widget::GetHeight();

    if (horizontal_) {
        if ((shift_x < 0) && (lt_corner[0] + shift_x < kArrowScrollBarHeight)) {
            parent->SetPercentage(0);
            Widget::Move(kArrowScrollBarHeight - lt_corner[0], 0);
            return;
        }
        if ((shift_x > 0) && (lt_corner[0] + shift_x + width > parent_width - kArrowScrollBarHeight)) {
            parent->SetPercentage(100 * (parent_width - 2 * kArrowScrollBarHeight - width)
                                         / (parent_width - 2 * kArrowScrollBarHeight));
            Widget::Move(parent_width - kArrowScrollBarHeight - width - lt_corner[0], 0);
            return;
        }
        parent->SetPercentage(100 * (lt_corner[0] + shift_x - kArrowScrollBarHeight)
                                     / (parent_width - 2 * kArrowScrollBarHeight));
        Widget::Move(shift_x, 0);
    } else {
        if ((shift_y < 0) && (lt_corner[1] + shift_y < kArrowScrollBarHeight)) {
            parent->SetPercentage(0);
            Widget::Move(0, kArrowScrollBarHeight - lt_corner[1]);
            return;
        }
        if ((shift_y > 0) && (lt_corner[1] + shift_y + height > parent_height - kArrowScrollBarHeight)) {
            parent->SetPercentage(100 * (parent_height - 2 * kArrowScrollBarHeight - height)
                                         / (parent_height - 2 * kArrowScrollBarHeight));
            Widget::Move(0, parent_height - kArrowScrollBarHeight - height - lt_corner[1]);
            return;
        }
        parent->SetPercentage(100 * (lt_corner[1] + shift_y - kArrowScrollBarHeight)
                                     / (parent_height - 2 * kArrowScrollBarHeight));
        Widget::Move(0, shift_y);
    }
};
