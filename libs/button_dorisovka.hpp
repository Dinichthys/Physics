#ifndef BUTTON_DORISOVKA_HPP
#define BUTTON_DORISOVKA_HPP

#include "button.hpp"
#include "widget_title.hpp"
#include "scene_manager.hpp"

static colors::Color kButtonDorisovkaColor = colors::Color(49, 49, 49);

static const std::string kButtonDorisovkaName = "Draw";

class ButtonDorisovka : public Widget {
    private:
        Button* button_;

    public:
        ButtonDorisovka(const Coordinates& lt_corner, float width, hui::State* state, Widget* parent)
            :Widget(lt_corner - Coordinates(2, 0, kTitleHeight), width, kTitleHeight * 2, state, parent) {
            button_ = new Button(Coordinates(2, 0, kTitleHeight), width, kTitleHeight, kButtonDorisovkaName, kWidgetDefaultFontFileName,
                state, this, kButtonDorisovkaColor, kButtonDorisovkaColor, kTitleHeight, colors::kColorWhite);
        };

        virtual void Redraw() override {
            if (hidden_) {
                return;
            }
            button_->Redraw();
            Widget::Redraw();
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            if (Widget::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }

                auto children = state->ui->main_field_->GetChildren();
                for (auto child : children) {
                    SceneManager* manager = dynamic_cast<SceneManager*>(child);
                    if (manager != NULL) {
                        if (manager->dorisovka_ != NULL) {
                            manager->OnESC();
                        } else {
                            manager->OnLetterG();
                        }
                        break;
                    }
                }

                return true;
            }
            return false;
        };

        virtual bool OnMouseRelease(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            if (Widget::OnMouseRelease(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }
                return true;
            }
            return false;
        };

        virtual bool OnMouseEnter(const Coordinates& mouse_pos, const Coordinates& delta) override {
            if (hidden_) {
                return false;
            }
            if (Widget::OnMouseEnter(mouse_pos, delta)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }
                return true;
            }
            hidden_ = true;
            return false;
        };
};

#endif // BUTTON_DORISOVKA_HPP
