#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <stdexcept>
#include <string.h>

#include "colors.hpp"

#include "widget.hpp"
#include "my_assert.h"
#include "text.hpp"

static const colors::Color kDefaultButtonColor = colors::kColorBlue;
static const colors::Color kPressedColor = colors::kColorGreen;
static const colors::Color kReleaseColor = colors::kColorRed;
static const colors::Color kPanelColor = colors::Color(60, 56, 54);

static uint8_t kHoveredColorScale = 2;

static const float kButtonTextScale = 1.2;

static const float kTextInButtonShiftVer = 0.05f;
static const float kTextInButtonShiftHor = 0.1f;

class Button : public WidgetContainer {
    private:
        bool pressed_;
        dr4::Rectangle button_background_;
        colors::Color pressed_color_;
        colors::Color released_color_;

    public:
        explicit Button(const Button& other)
            :WidgetContainer(other),
             button_background_(dr4::Rect2f({0, 0}, {other.GetWidth(), other.GetHeight()}),
                                other.released_color_) {

            pressed_ = other.GetPressedInfo();
            pressed_color_ = other.GetPressedColor();
            released_color_ = other.GetReleasedColor();

            Text* text = dynamic_cast<Text*>(other.GetChild(0));
            if (text == NULL) {
                return;
            }

            WidgetContainer::SetChild(0, new(std::nothrow) Text(*text));
            if (WidgetContainer::GetChild(0) == NULL) {
                throw std::runtime_error("Can't do copy constructor for Button\n");
            }
        };

        explicit Button(const Coordinates& lt_corner, float width, float height,
                        const std::string& text = "", const std::string& file_name = "",
                        hui::State* state = NULL, Widget* parent = NULL,
                        colors::Color pressed_color = kPressedColor,
                        colors::Color released_color = kReleaseColor,
                        float character_size = 0)
            :WidgetContainer(lt_corner, width, height, state),
             button_background_(dr4::Rect2f({0, 0}, {width, height}), released_color),
             pressed_color_(pressed_color), released_color_(released_color) {
            pressed_ = false;

            if ((strcmp(text.c_str(), "") != 0) && (strcmp(file_name.c_str(), "") != 0)) {
                WidgetContainer::AddChild(new(std::nothrow) Text(
                                                            Coordinates(2, 0, 0),
                                                            width, height / kButtonTextScale, state, this,
                                                            text, file_name, character_size));
                if (WidgetContainer::GetChildren().back() == NULL) {
                    throw std::runtime_error("Bad allocation for text");
                }
            }

            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        virtual void SetSize(dr4::Vec2f size) override {
            Widget::SetSize(size);
            if (WidgetContainer::GetChildrenNum() != 0) {
                WidgetContainer::GetChild(0)->SetSize(size);
            };
            button_background_.rect.size = size;
        };

        void SetText(const std::string& text_str) {
            Text* text = dynamic_cast<Text*>(GetChild(0));
            if (text == NULL) {
                return;
            }
            text->SetText(text_str);
        }

        bool GetPressedInfo() const {return pressed_;};
        void SetPressedInfo(bool new_pressed) {pressed_ = new_pressed;};

        colors::Color GetPressedColor() const {return pressed_color_;};
        colors::Color GetReleasedColor() const {return released_color_;};

        virtual void Redraw() override {
            colors::Color color = (Button::GetPressedInfo()) ? pressed_color_ : released_color_;

            if (Widget::GetHovered()) {
                color  = colors::Color(color.GetRedPart() / kHoveredColorScale,
                                         color.GetBluePart() / kHoveredColorScale,
                                         color.GetGreenPart() / kHoveredColorScale,
                                         color.GetBrightness());
            }

            button_background_.fill = color;
            texture->Draw(button_background_);

            WidgetContainer::Redraw();
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();

            state->target_widget_ = NULL;

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {
                return true;
            }

            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos) override {
            Coordinates lt_corner(Widget::GetLTCornerLoc());
            float width = Widget::GetWidth();
            float height = Widget::GetHeight();
            pressed_ = false;

            if ((mouse_pos[0] > lt_corner[0])
                && (mouse_pos[1] > lt_corner[1])
                && (mouse_pos[0] < lt_corner[0] + width)
                && (mouse_pos[1] < lt_corner[1] + height)) {

                return true;
            }

            return false;
        };
};

class PanelControl : public WidgetContainer {
    private:
        dr4::Rectangle background_;

    public:
        explicit PanelControl(const PanelControl& other)
            :WidgetContainer(other),
             background_(dr4::Rect2f({0, 0}, {other.GetWidth(), other.GetHeight()}), kPanelColor) {
            Widget::SetParent(other.GetParent());
            WidgetContainer::SetParentToChildren();
        };

        explicit PanelControl(const Coordinates& lt_corner = Coordinates(3), float width = 0, float height = 0,
                              hui::State* state = NULL,
                              const std::vector<Widget*>* buttons = NULL, Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height, state, buttons),
             background_(dr4::Rect2f({0, 0}, {width, height}), kPanelColor) {
            Widget::SetParent(parent);
            WidgetContainer::SetParentToChildren();
        };

        virtual void Redraw() override {
            if (Widget::GetHidden()) {
                return;
            }
            texture->Draw(background_);

            WidgetContainer::Redraw();
        };
};

#endif // BUTTON_HPP
