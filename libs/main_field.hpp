#ifndef MAIN_FIELD_HPP
#define MAIN_FIELD_HPP

#include "widget.hpp"

static const colors::Color kBackgroundMainFieldColor = colors::Color(50, 50, 50);
static const colors::Color kBorderMainFieldColor = kWidgetDefaultBorderColor;
static const size_t kBorderMainFieldThickness = kBorderThicknessWidget;

class MainField : public WidgetContainer {
    private:
        dr4::Rectangle* background_;

    public:
        MainField(const Coordinates& lt_corner, float width, float height, hui::State* state,
                  const std::vector<Widget*>* children = NULL, Widget* parent = NULL)
            :WidgetContainer(lt_corner, width, height, state, children, parent) {
            if (state != NULL) {
                background_ = state->window_->CreateRectangle();
                background_->SetFillColor(kBackgroundMainFieldColor);
                background_->SetBorderColor(kBorderMainFieldColor);
                background_->SetBorderThickness(kBorderMainFieldThickness);
                background_->SetPos({kBorderMainFieldThickness, kBorderMainFieldThickness});
                background_->SetSize(dr4::Vec2f{width, height} - dr4::Vec2f{kBorderMainFieldThickness, kBorderMainFieldThickness} * 2);
            }

            WidgetContainer::SetParentToChildren();
        };

        virtual void SetState(hui::State* state_) override {
            WidgetContainer::SetState(state_);

            background_ = state->window_->CreateRectangle();
            background_->SetFillColor(kBackgroundMainFieldColor);
            background_->SetBorderColor(kBorderMainFieldColor);
            background_->SetBorderThickness(kBorderMainFieldThickness);
            background_->SetPos({kBorderMainFieldThickness, kBorderMainFieldThickness});
            background_->SetSize(Widget::GetSize() - dr4::Vec2f{kBorderMainFieldThickness, kBorderMainFieldThickness} * 2);
        }

        virtual void Redraw() override {
            background_->DrawOn(*texture);

            WidgetContainer::Redraw();
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (WidgetContainer::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                    return false;
                }
                return true;
            }
            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (WidgetContainer::OnMouseRelease(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                    return false;
                }
                return true;
            }
            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (WidgetContainer::OnMouseEnter(mouse_pos, delta)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                    return false;
                }
                return true;
            }
            return false;
        };
};

#endif // MAIN_FIELD_HPP
