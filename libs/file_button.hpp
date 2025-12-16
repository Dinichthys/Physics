#ifndef FILE_BUTTON_HPP
#define FILE_BUTTON_HPP

#include "draw.hpp"
#include "button.hpp"
#include "button_dorisovka.hpp"

static const std::string kFileButtonName = "File";

class FileButton : public Button {
    private:
        ButtonDorisovka* dorisovka_;

    public:
        FileButton(const Coordinates& lt_corner, float width, hui::State* state, Widget* parent)
            :Button(lt_corner, width, kTitleHeight, kFileButtonName, kFontFileNameTitle, state, parent,
                kTitleBackgroundColor, kTitleBackgroundColor) {dorisovka_ = NULL;};

        virtual void SetState(hui::State* state_) override {
            Button::SetState(state_);
        }

        virtual bool OnMousePress(const Coordinates& mouse_pos, const dr4::MouseButtonType type) override {
            if (WidgetContainer::OnMousePress(mouse_pos, type)) {
                if (state->target_widget_ == this) {
                    state->target_widget_ = NULL;
                }

                if (dorisovka_) {
                    dorisovka_->SetHidden(!(dorisovka_->GetHidden()));
                    return true;
                }

                dorisovka_ = new ButtonDorisovka(Widget::GetLTCornerLoc() + Coordinates(2, 0, kTitleHeight), Widget::GetWidth(), state, state->ui);
                state->ui->AddChild(dorisovka_);

                return true;
            }
            return false;
        };
};

#endif // FILE_BUTTON_HPP
