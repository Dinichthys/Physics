#ifndef SCROLLBAR_HPP
#define SCROLLBAR_HPP

#include <iostream>
#include <functional>

#include "widget.hpp"
#include "button.hpp"
#include "vector.hpp"

enum ScrollBarElemIdx {
    kThumb = 0,
    kPlusArrow = 1,
    kMinusArrow = 2,
};

static const std::string kFontFileNameScrollBar = "data/font.ttf";
static const std::string kArrowUpStr = "/\\";
static const std::string kArrowDownStr = "\\/";
static const std::string kArrowRightStr = ">";
static const std::string kArrowLeftStr = "<";
static const float kArrowScrollBarHeight = 20.f;
static const float kDeltaArrow = 10;

static const graphics::Color kThumbColor = graphics::Color(85, 85, 89);
static const graphics::Color kScrollBarColor = graphics::Color(56, 56, 59);
static const graphics::Color kArrowsColor = graphics::Color(140, 140, 140);

class ScrollBar;

class ArrowScrollBar : public Button {
    private:
        float delta_;

    public:
        explicit ArrowScrollBar(const Button& button, float delta)
            :Button(button) {
            delta_ = delta;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override;
};

class Thumb : public Button {
    private:
        bool horizontal_;

    public:
        explicit Thumb(const Coordinates& lt_corner, float width, float height, bool horizontal)
            :Button(lt_corner, width, height, "", "", NULL, kThumbColor, kThumbColor) {
            horizontal_ = horizontal;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            return Widget::OnMousePress(mouse_pos);
        };

        virtual void Move(float shift_x, float shift_y) override;

        virtual bool OnMouseMove(float shift_x, float shift_y) override {
            return Widget::OnMouseMove(shift_x, shift_y);
        };
};

class ScrollBar : public Button {
    private:
        float percentage_;
        float delta_;
        std::function<void(float)> action_;

    public:
        explicit ScrollBar(const Coordinates& lt_corner = Coordinates(3),
                           float width = 0, float height = 0, float thumb_height = 0,
                           std::function<void(float)> action = std::function<void(float)>(),
                           float delta = kDeltaArrow)
            :Button(lt_corner, width, height, "", "", NULL, kScrollBarColor, kScrollBarColor), action_(action) {
            delta_ = delta;

            try {
                if (width < height) {
                    WidgetContainer::AddChild(new Thumb(Coordinates(2, 0, kArrowScrollBarHeight),
                                                        width, thumb_height, false));
                    WidgetContainer::AddChild(new ArrowScrollBar(
                                                  Button(Coordinates(2, 0, 0),
                                                         width, kArrowScrollBarHeight,
                                                         kArrowUpStr, kFontFileNameScrollBar,
                                                         this, kArrowsColor, kArrowsColor), -kDeltaArrow));
                    WidgetContainer::AddChild(new ArrowScrollBar(
                                                  Button(Coordinates(2, 0, height - kArrowScrollBarHeight),
                                                         width, kArrowScrollBarHeight,
                                                         kArrowDownStr, kFontFileNameScrollBar,
                                                         this, kArrowsColor, kArrowsColor), kDeltaArrow));
                } else {
                    WidgetContainer::AddChild(new Thumb(Coordinates(2, kArrowScrollBarHeight, 0),
                                                        thumb_height, height, true));
                    WidgetContainer::AddChild(new ArrowScrollBar(
                                                  Button(Coordinates(2, 0, 0),
                                                         kArrowScrollBarHeight, height,
                                                         kArrowLeftStr, kFontFileNameScrollBar,
                                                         this, kArrowsColor, kArrowsColor), -kDeltaArrow));
                    WidgetContainer::AddChild(new ArrowScrollBar(
                                                  Button(Coordinates(2, width - kArrowScrollBarHeight, 0),
                                                         kArrowScrollBarHeight, height,
                                                         kArrowRightStr, kFontFileNameScrollBar,
                                                         this, kArrowsColor, kArrowsColor), kDeltaArrow));
                }
            } catch (const std::bad_alloc& e) {
                std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            }

            percentage_ = 0;
            WidgetContainer::SetParentToChildren();
        };

        void SetPercentage(float percentage) {
            float old_percentage = percentage_;
            percentage_ = (percentage > 100) ? 100 : percentage;
            action_(percentage_ - old_percentage);
        };
        void AddPercentage(float delta) {
            SetPercentage(percentage_ + delta);
        };

        float GetPercentage() const {
            return percentage_;
        };

        void SetThumbSize(float width, float height) {
            WidgetContainer::GetChild(kThumb)->SetSize(width, height);
        };
        void SetArrowsSize(float width, float height) {
            WidgetContainer::GetChild(kPlusArrow)->SetSize(width, height);
            WidgetContainer::GetChild(kMinusArrow)->SetSize(width, height);
        };
        void SetAction(const std::function<void(float)>& action) {
            action_ = action;
        };
        void SetDelta(float delta) {
            delta_ = delta;
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            if (WidgetContainer::OnMousePress(mouse_pos)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                    Button::SetPressedInfo(true);

                    Thumb* thumb = GetThumb();
                    if (thumb == NULL) {
                        return true;
                    }

                    Coordinates lt_thumb(thumb->GetLTCornerLoc());
                    Coordinates loc_mouse_pos = mouse_pos - Widget::GetLTCornerLoc();
                    if (lt_thumb[0] < kEpsilon) {
                        if (lt_thumb[1] > loc_mouse_pos[1]) {
                            AddPercentage(-delta_);
                            MoveThumb(-delta_);
                        } else {
                            AddPercentage(delta_);
                            MoveThumb(delta_);
                        }
                    } else {
                        if (lt_thumb[0] > loc_mouse_pos[0]) {
                            AddPercentage(-delta_);
                            MoveThumb(-delta_);
                        } else {
                            AddPercentage(delta_);
                            MoveThumb(delta_);
                        }
                    }
                }
                return true;
            }
            Button::SetPressedInfo(false);
            return false;
        };

        void MoveThumb(float percentage) {
            Thumb* thumb = GetThumb();
            if (thumb == NULL) {
                return;
            }
            thumb->Move(percentage * Widget::GetWidth() / 100, percentage * Widget::GetHeight() / 100);
        };

    private:
        Thumb* GetThumb() const {
            return dynamic_cast<Thumb*>(WidgetContainer::GetChild(kThumb));
        };
        ArrowScrollBar* GetPlusArrowScrollBar() const {
            return dynamic_cast<ArrowScrollBar*>(WidgetContainer::GetChild(kPlusArrow));
        };
        ArrowScrollBar* GetMinusArrowScrollBar() const {
            return dynamic_cast<ArrowScrollBar*>(WidgetContainer::GetChild(kMinusArrow));
        };
};

#endif // SCROLLBAR_HPP
