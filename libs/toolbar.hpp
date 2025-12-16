#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include <vector>

#include "widget.hpp"
#include "button.hpp"
#include "widget_title.hpp"
#include "color_picker.hpp"
#include "draw.hpp"

static const size_t kWidthToolBar = 75;
static const size_t kSpaceBetweenTools = 2;
static const size_t kToolButtonSize = (kWidthToolBar - 3 * kSpaceBetweenTools) / 2;
static const size_t kColorBoxSize = kWidthToolBar / 2 - 2 * kSpaceBetweenTools;

static const std::string kToolBarName = "Tools";

class Dorisovka;
class ToolBar : public Widget {
    private:
        Dorisovka* cvs_;
        std::vector<Button*> buttons_;

        Title* title_;

        dr4::Texture* color_box_;

        dr4::Rectangle* background_;

        ColorPicker* color_picker_;

    public:
        ToolBar(const Coordinates& lt_corner, Dorisovka* cvs, hui::State* state, Widget* parent);

        ~ToolBar() {
            delete title_;
            delete color_box_;
            delete background_;
            if (state->target_widget_ == title_) {
                state->target_widget_ = NULL;
            }

            auto children = state->ui->GetChildren();
            for (size_t i = 0; i < children.size(); i++) {
                if (children[i] == color_picker_) {
                    state->ui->GetChildren().erase(children.begin() + i);
                    delete color_picker_;
                }
            }
        }

        void AddButton(Button* button) {
            buttons_.push_back(button);
            button->SetRelPos({float(kSpaceBetweenTools
                + ((((buttons_.size() - 1) % 2) == 0)
                    ? 0 : kSpaceBetweenTools + kToolButtonSize)), float(kTitleHeight + kSpaceBetweenTools + (kToolButtonSize + kSpaceBetweenTools) * ((buttons_.size() - 1) / 2))});

            button->SetSize({kToolButtonSize, kToolButtonSize});

            button->SetParent(this);

            if ((buttons_.size() - 1) % 2 == 0) {
                color_box_->SetPos({color_box_->GetPos().x, color_box_->GetPos().y + kToolButtonSize + kSpaceBetweenTools});
                Widget::SetSize({Widget::GetSize().x, Widget::GetSize().y + kToolButtonSize + kSpaceBetweenTools});
                background_->SetSize(background_->GetSize() + dr4::Vec2f(0, kToolButtonSize + kSpaceBetweenTools));
            }
        };

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (hidden_) {
                return false;
            }
            Coordinates loc_coors(2, mouse_pos[0] - relPos.x, mouse_pos[1] - relPos.y);
            if ((mouse_pos[0] > relPos.x)
                && (mouse_pos[1] > relPos.y)
                && (mouse_pos[0] < relPos.x + width_)
                && (mouse_pos[1] < relPos.y + height_)) {
                if (title_->OnMousePress(loc_coors, type)) {
                    return true;
                }

                for (auto button : buttons_) {
                    if (button->OnMousePress(loc_coors, type)) {
                        state->target_widget_ = NULL;
                        return true;
                    }
                }

                auto color_box_pos = color_box_->GetPos();
                if ((loc_coors[0] > color_box_pos.x)
                    && (loc_coors[1] > color_box_pos.y)
                    && (loc_coors[0] < color_box_pos.x + kColorBoxSize)
                    && (loc_coors[1] < color_box_pos.y + kColorBoxSize)) {
                    if (color_picker_ != NULL) {
                        auto children = state->ui->GetChildren();
                        for (size_t i = 0; i < children.size(); i++) {
                            if (children[i] == color_picker_) {
                                state->ui->GetChildren().erase(children.begin() + i);
                                delete color_picker_;
                            }
                        }
                        color_picker_ = NULL;
                        return true;
                    }

                    color_picker_ = new ColorPicker(Coordinates(2, 0, 0), state, this, cvs_);
                    state->ui->AddChild(color_picker_);
                    state->target_widget_ = NULL;
                    return true;
                }

                state->target_widget_ = NULL;
                return true;
            }

            return false;
        };

        virtual void Redraw() override {
            background_->DrawOn(*texture);

            for (auto button : buttons_) {
                button->Redraw();
            }

            color_box_->DrawOn(*texture);

            title_->Redraw();
            Widget::Redraw();
        };
};

#endif // TOOLBAR_HPP
