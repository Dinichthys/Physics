#ifndef CAMERA_CONTROL_HPP
#define CAMERA_CONTROL_HPP

#include <string>
#include <functional>

#include "colors.hpp"

#include "widget.hpp"
#include "button.hpp"
#include "widget_title.hpp"

static const std::string kCameraControlTitle = "Camera";

class SceneManager;

class CameraController : public WidgetContainer {
    private:
        dr4::Rectangle* rect_;

        Title* title_;

    public:
        explicit CameraController(const Coordinates& lt_corner, float width, float height,
                           hui::State* state,
                           Widget* parent = NULL, const std::vector<Widget*>* buttons = NULL)
            :WidgetContainer(lt_corner, width, height, state, buttons, parent),
            rect_((state == NULL) ? NULL : state->window_->CreateRectangle()) {
            Widget::SetLTCorner(Coordinates(2, relPos.x, relPos.y + kTitleHeight));
            if (rect_ != NULL) {
                rect_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget + kTitleHeight});
                rect_->SetSize(dr4::Vec2f{width, height} - dr4::Vec2f{kBorderThicknessWidget, kBorderThicknessWidget} * 2);
                rect_->SetFillColor(kTitleBackgroundColor);
                rect_->SetBorderColor(kTitleBorderColor);
                rect_->SetBorderThickness(kBorderThicknessWidget);
            }

            title_ = new Title(Coordinates(2), width, state, this, kCameraControlTitle, this);

            SetParentToChildren();
        };

        ~CameraController() {
            delete rect_;
        }

        void SetState(hui::State* state_) {
            WidgetContainer::SetState(state_);

            if (rect_ == NULL) {
                delete rect_;
            }

            rect_ = state->window_->CreateRectangle();

            rect_->SetPos({kBorderThicknessWidget, kBorderThicknessWidget + kTitleHeight});
            rect_->SetSize(Widget::GetSize() - dr4::Vec2f{kBorderThicknessWidget, kBorderThicknessWidget} * 2);
            rect_->SetFillColor(kTitleBackgroundColor);
            rect_->SetBorderColor(kTitleBorderColor);
            rect_->SetBorderThickness(kBorderThicknessWidget);

            texture->SetSize(Widget::GetWidth(), Widget::GetHeight() + kTitleHeight);
            Widget::SetSize({Widget::GetWidth(), Widget::GetHeight() + kTitleHeight});
            texture->SetPos({relPos.x, relPos.y - kTitleHeight});

            title_->SetState(state);
        };

        virtual void Redraw() override {
            if (hidden_) {
                return;
            }
            rect_->DrawOn(*texture);
            title_->Redraw();
            WidgetContainer::Redraw();
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) {
            if (hidden_) {
                return false;
            }
            if (title_->OnMousePress(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }
            return WidgetContainer::OnMousePress(mouse_pos, type);
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (title_->OnMouseEnter(mouse_pos - Widget::GetLTCornerLoc(), delta)) {
                return true;
            }
            return WidgetContainer::OnMouseEnter(mouse_pos, delta);
        }

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (title_->OnMouseRelease(mouse_pos - Widget::GetLTCornerLoc(), type)) {
                return true;
            }
            return WidgetContainer::OnMouseRelease(mouse_pos, type);
        }

        virtual const std::string& GetName() const override {
            return kCameraControlTitle;
        };
};

class CameraControlButton : public Button {
    private:
        std::function<void(void)> action_;

    public:
        explicit CameraControlButton(const Button& button, const std::function<void(void)>& action)
            :Button(button), action_(action) {};

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (!Button::OnMousePress(mouse_pos, type)) {
                return false;
            }
            action_();
            return true;
        };
};

#endif // CAMERA_CONTROL_HPP
